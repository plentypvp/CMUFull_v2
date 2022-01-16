#include "dsp.h"

static float32_t l_biquad_state[IIR_ORDER];
static float32_t l_biquad_coeffs[5*IIR_NUMSTAGES] =
        {
                0.000000000061177,
                0.000000000122354,
                0.000000000061177,
                1.923683262129048,
                -0.925239643276598,
                1.000000000000000,
                2.000000000000000,
                1.000000000000000,
                1.943145788201428,
                -0.944717915760698,
                1.000000000000000,
                2.000000000000000,
                1.000000000000000,
                1.977804273572809,
                -0.979404442033768
        };
static float32_t m_biquad_state[IIR_ORDER];
static float32_t m_biquad_coeffs[5*IIR_NUMSTAGES] =
        {
                0.01618,
                -0.01358,
                0.01618,
                1.57133,
                -0.88023,
                1.00000,
                -1.99655,
                1.00000,
                1.75546,
                -0.82268,
                1.00000,
                0.00000,
                -1.00000,
                1.95590,
                -0.97139
        };
static float32_t h_biquad_state[IIR_ORDER];
static float32_t h_biquad_coeffs[5*IIR_NUMSTAGES] =
        {
                0.16065,
                -0.32131,
                0.16065,
                0.70231,
                -0.13669,
                1.00000,
                -2.00000,
                1.00000,
                0.79417,
                -0.28536,
                1.00000,
                -2.00000,
                1.00000,
                1.02677,
                -0.66181
        };
arm_biquad_cascade_df2T_instance_f32 const l_iir_inst =
        {
                IIR_ORDER/2,
                l_biquad_state,
                l_biquad_coeffs
        };
arm_biquad_cascade_df2T_instance_f32 const m_iir_inst =
        {
                IIR_ORDER/2,
                m_biquad_state,
                m_biquad_coeffs
        };
arm_biquad_cascade_df2T_instance_f32 const h_iir_inst =
        {
                IIR_ORDER/2,
                h_biquad_state,
                h_biquad_coeffs
        };

// DC blocker
static float32_t dcb_biquad_state[2];
static float32_t dcb_biquad_coeffs[5] =
        {
                0.99787,
                -1.99574,
                0.99787,
                1.99573,
                -0.99574
        };
arm_biquad_cascade_df2T_instance_f32 const dcb_iir_inst =
        {
                1,
                dcb_biquad_state,
                dcb_biquad_coeffs
        };

// Sampling
float32_t sample_buffer[SAMPLE_BUF_SIZE];
float32_t dc_blocked[SAMPLE_BUF_SIZE];
float32_t l_filtered[SAMPLE_BUF_SIZE * INTEGRATOR_SIZE];
float32_t m_filtered[SAMPLE_BUF_SIZE * INTEGRATOR_SIZE];
float32_t h_filtered[SAMPLE_BUF_SIZE * INTEGRATOR_SIZE];

// Amplitude levels and ADC
static nrf_saadc_value_t samples[SAADC_BUF_COUNT][SAMPLE_BUF_SIZE];
static nrfx_saadc_channel_t channel_config = NRFX_SAADC_DEFAULT_CHANNEL_SE(NRF_SAADC_INPUT_AIN7, 0);

// Settings
// 0.53025 volts RMS of input signal, that is 0.15 * 4096 = 603, 650 for square wave
dsp_settings_s dspSettings = {
        .redIntegrator = 3,
        .redMinAmplitude = 30,
        .redMaxAmplitude = 650,
        .greenIntegrator = 3,
        .greenMinAmplitude = 30,
        .greenMaxAmplitude = 650,
        .blueIntegrator = 3,
        .blueMinAmplitude = 30,
        .blueMaxAmplitude = 650
};

float map(float x, float in_min, float in_max, float out_min, float out_max)
{
    if (out_max == 0.0f) return 0.0f;
    if (x < in_min) x = in_min;
    if (x > in_max) x = in_max;
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static uint32_t next_free_buf_index(void)
{
    static uint32_t buffer_index = -1;
    buffer_index = (buffer_index + 1) % SAADC_BUF_COUNT;
    return buffer_index;
}

static void event_handler(nrfx_saadc_evt_t const * p_event)
{
    ret_code_t err_code;
    switch (p_event->type)
    {
        case NRFX_SAADC_EVT_DONE:
            // Block DC
            for (int i = 0; i < SAMPLE_BUF_SIZE; i++) sample_buffer[i] = p_event->data.done.p_buffer[i];
            arm_biquad_cascade_df2T_f32(&dcb_iir_inst, sample_buffer, dc_blocked, SAMPLE_BUF_SIZE);

            // Process buffer and add to the end of integrator
            memmove(l_filtered, &l_filtered[SAMPLE_BUF_SIZE], sizeof(l_filtered) - (sizeof(float32_t) * SAMPLE_BUF_SIZE));
            memmove(m_filtered, &m_filtered[SAMPLE_BUF_SIZE], sizeof(m_filtered) - (sizeof(float32_t) * SAMPLE_BUF_SIZE));
            memmove(h_filtered, &h_filtered[SAMPLE_BUF_SIZE], sizeof(h_filtered) - (sizeof(float32_t) * SAMPLE_BUF_SIZE));
            arm_biquad_cascade_df2T_f32(&l_iir_inst, dc_blocked, &l_filtered[SAMPLE_BUF_SIZE * (INTEGRATOR_SIZE - 1)], SAMPLE_BUF_SIZE);
            arm_biquad_cascade_df2T_f32(&m_iir_inst, dc_blocked, &m_filtered[SAMPLE_BUF_SIZE * (INTEGRATOR_SIZE - 1)], SAMPLE_BUF_SIZE);
            arm_biquad_cascade_df2T_f32(&h_iir_inst, dc_blocked, &h_filtered[SAMPLE_BUF_SIZE * (INTEGRATOR_SIZE - 1)], SAMPLE_BUF_SIZE);

            // Process filtered data
            float32_t l_rms = 0.0f;
            float32_t m_rms = 0.0f;
            float32_t h_rms = 0.0f;
            arm_rms_f32(&l_filtered[SAMPLE_BUF_SIZE * (INTEGRATOR_SIZE - dspSettings.redIntegrator)], SAMPLE_BUF_SIZE * dspSettings.redIntegrator, &l_rms);
            arm_rms_f32(&m_filtered[SAMPLE_BUF_SIZE * (INTEGRATOR_SIZE - dspSettings.greenIntegrator)], SAMPLE_BUF_SIZE * dspSettings.greenIntegrator, &m_rms);
            arm_rms_f32(&h_filtered[SAMPLE_BUF_SIZE * (INTEGRATOR_SIZE - dspSettings.blueIntegrator)], SAMPLE_BUF_SIZE * dspSettings.blueIntegrator, &h_rms);

            // Calculate LED brightness
            float redLedBrightness = map(l_rms, dspSettings.redMinAmplitude, dspSettings.redMaxAmplitude, 0.0f, 1.0f);
            float greenLedBrightness = map(m_rms, dspSettings.greenMinAmplitude, dspSettings.greenMaxAmplitude, 0.0f, 1.0f);
            float blueLedBrightness = map(h_rms, dspSettings.blueMinAmplitude, dspSettings.blueMaxAmplitude, 0.0f, 1.0f);

            // Display result
            neopixel_set_cmu_color(redLedBrightness, greenLedBrightness, blueLedBrightness);
            break;

        case NRFX_SAADC_EVT_BUF_REQ:
            // Set up the next available buffer
            err_code = nrfx_saadc_buffer_set(&samples[next_free_buf_index()][0], SAMPLE_BUF_SIZE);
            APP_ERROR_CHECK(err_code);
            break;

        case NRFX_SAADC_EVT_LIMIT:
            break;

        case NRFX_SAADC_EVT_CALIBRATEDONE:
            break;

        case NRFX_SAADC_EVT_READY:
            break;

        case NRFX_SAADC_EVT_FINISHED:
            break;
    }
}

void adc_start(uint32_t cc_value)
{
    ret_code_t err_code;

    nrfx_saadc_adv_config_t saadc_adv_config = NRFX_SAADC_DEFAULT_ADV_CONFIG;
    saadc_adv_config.internal_timer_cc = cc_value;
    saadc_adv_config.start_on_end = true;
    saadc_adv_config.oversampling = NRF_SAADC_OVERSAMPLE_4X;

    err_code = nrfx_saadc_advanced_mode_set((1<<0),
                                            NRF_SAADC_RESOLUTION_12BIT,
                                            &saadc_adv_config,
                                            event_handler);
    APP_ERROR_CHECK(err_code);

    // Configure two buffers to ensure double buffering of samples, to avoid data loss when the sampling frequency is high
    err_code = nrfx_saadc_buffer_set(&samples[next_free_buf_index()][0], SAMPLE_BUF_SIZE);
    APP_ERROR_CHECK(err_code);

    err_code = nrfx_saadc_buffer_set(&samples[next_free_buf_index()][0], SAMPLE_BUF_SIZE);
    APP_ERROR_CHECK(err_code);

    err_code = nrfx_saadc_mode_trigger();
    APP_ERROR_CHECK(err_code);
}

// ADC-related stuff
void saadc_init(void)
{
    ret_code_t err_code;
    err_code = nrfx_saadc_init(NRFX_SAADC_CONFIG_IRQ_PRIORITY);
    APP_ERROR_CHECK(err_code);

    channel_config.channel_config.acq_time = NRF_SAADC_ACQTIME_3US;
    err_code = nrfx_saadc_channels_config(&channel_config, 1);
    APP_ERROR_CHECK(err_code);

    uint32_t adc_cc_value = 4000000 / SAADC_SAMPLE_FREQUENCY;
    adc_start(adc_cc_value);
}

dsp_settings_s dsp_get_settings() {
    return dspSettings;
}

void dsp_set_settings(dsp_settings_s new_settings) {
    // Verify red
    if (new_settings.redIntegrator > 5) new_settings.redIntegrator = 5;
    if (new_settings.redIntegrator < 1) new_settings.redIntegrator = 1;
    if (new_settings.redMinAmplitude < 0) new_settings.redMinAmplitude = 0;
    if (new_settings.redMinAmplitude > 4096) new_settings.redMinAmplitude = 4096;
    if (new_settings.redMaxAmplitude < 0) new_settings.redMaxAmplitude = 0;
    if (new_settings.redMaxAmplitude > 4096) new_settings.redMaxAmplitude = 4096;
    if (new_settings.redMaxAmplitude < new_settings.redMinAmplitude) {
        new_settings.redMinAmplitude = 0;
        new_settings.redMaxAmplitude = 4096;
    }

    // Verify green
    if (new_settings.greenIntegrator > 5) new_settings.greenIntegrator = 5;
    if (new_settings.greenIntegrator < 1) new_settings.greenIntegrator = 1;
    if (new_settings.greenMinAmplitude < 0) new_settings.greenMinAmplitude = 0;
    if (new_settings.greenMinAmplitude > 4096) new_settings.greenMinAmplitude = 4096;
    if (new_settings.greenMaxAmplitude < 0) new_settings.greenMaxAmplitude = 0;
    if (new_settings.greenMaxAmplitude > 4096) new_settings.greenMaxAmplitude = 4096;
    if (new_settings.greenMaxAmplitude < new_settings.greenMinAmplitude) {
        new_settings.greenMinAmplitude = 0;
        new_settings.greenMaxAmplitude = 4096;
    }

    // Verify blue
    if (new_settings.blueIntegrator > 5) new_settings.blueIntegrator = 5;
    if (new_settings.blueIntegrator < 1) new_settings.blueIntegrator = 1;
    if (new_settings.blueMinAmplitude < 0) new_settings.blueMinAmplitude = 0;
    if (new_settings.blueMinAmplitude > 4096) new_settings.blueMinAmplitude = 4096;
    if (new_settings.blueMaxAmplitude < 0) new_settings.blueMaxAmplitude = 0;
    if (new_settings.blueMaxAmplitude > 4096) new_settings.blueMaxAmplitude = 4096;
    if (new_settings.blueMaxAmplitude < new_settings.blueMinAmplitude) {
        new_settings.blueMinAmplitude = 0;
        new_settings.blueMaxAmplitude = 4096;
    }

    dspSettings = new_settings;
}