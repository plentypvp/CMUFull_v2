#include "neopixel.h"

#define NRFX_PWM_DUTY_INVERTED 0x8000
#define BIT0 (NRFX_PWM_DUTY_INVERTED | 5)
#define BIT1 (NRFX_PWM_DUTY_INVERTED | 14)

uint16_t seq_values[BITS_PER_PIXEL * PIXEL_COUNT] = { BIT0 };
const nrfx_pwm_t m_pwm = NRFX_PWM_INSTANCE(0);

neopixel_settings_s neopixelSettings = {
    .neopixelMode = NEOPIXEL_INITIAL_MODE,
    .maxRedBrightness = 255,
    .maxGreenBrightness = 70,
    .maxBlueBrightness = 70
};

void neopixel_init() {
    ret_code_t ret_code;

    nrf_pwm_clk_t pwm_clock = NRF_PWM_CLK_16MHz;
    uint16_t period_hwu = 20; // 800 kHz with 16 MHz clock

    nrfx_pwm_config_t const config = {
        .output_pins =
        {
            NEOPIXEL_PIN
        },
        .irq_priority = APP_IRQ_PRIORITY_LOWEST,
        .base_clock = pwm_clock,
        .count_mode = NRF_PWM_MODE_UP,
        .top_value = period_hwu,
        .load_mode = NRF_PWM_LOAD_COMMON,
        .step_mode = NRF_PWM_STEP_AUTO
    };

    ret_code = nrfx_pwm_init(&m_pwm, &config, neopixel_sequence_handler);
    if (ret_code)
    {
        return;
    }

    neopixel_update();
}

void neopixel_update() {
    ret_code_t ret_code;

    nrf_pwm_sequence_t const seq = {
        .values = { .p_common = seq_values },
        .length = NRF_PWM_VALUES_LENGTH(seq_values),
        .repeats = 0,
        .end_delay = 0
    };

    ret_code = nrfx_pwm_simple_playback(&m_pwm, &seq, 1, NRFX_PWM_FLAG_STOP);
    if (ret_code)
    {
        return;
    }
}

void neopixel_sequence_handler(nrfx_pwm_evt_type_t event_type) {
    if (event_type == NRFX_PWM_EVT_STOPPED) {

    }
}

void neopixel_set_color(uint8_t i, uint8_t r, uint8_t g, uint8_t b) {
    uint32_t color = (g << 16) | (r << 8) | b;
    uint16_t* p = &seq_values[i * BITS_PER_PIXEL];
    uint32_t mask = 0x00800000;
    while (mask) {
        if (color & mask) {
            *p = BIT1;
        } else {
            *p = BIT0;
        }
        mask >>= 1;
        p++;
    }
}

void neopixel_colorwipe(uint8_t r, uint8_t g, uint8_t b) {
    for (uint16_t i = 0; i < PIXEL_COUNT; i++) {
        neopixel_set_color(i, r, g, b);
        neopixel_update();
        nrf_delay_ms(10);
    }
}

void neopixel_set_cmu_color(float r, float g, float b) {
    // Here come values 0..1.0, spectrally correct them and adapt to max brightness
    uint8_t redLed = (uint8_t)(r * (float)neopixelSettings.maxRedBrightness);
    uint8_t greenLed = (uint8_t)(g * (float)neopixelSettings.maxGreenBrightness);
    uint8_t blueLed = (uint8_t)(b * (float)neopixelSettings.maxBlueBrightness);
    // Set pixel colors according to the mode
    if (neopixelSettings.neopixelMode == 0) {
        for (uint16_t i = 0; i < PIXEL_COUNT; i++) {
            neopixel_set_color(i, redLed, greenLed, blueLed);
        }
    } else if (neopixelSettings.neopixelMode == 1) {
        int perChannel = PIXEL_COUNT / 3;
        for (int j = 0; j < perChannel; j++) {
            neopixel_set_color(j, redLed, 0, 0);
        }
        for (int j = perChannel; j < (perChannel * 2); j++) {
            neopixel_set_color(j, 0, greenLed, 0);
        }
        for (int j = (perChannel * 2); j < (perChannel * 3); j++) {
            neopixel_set_color(j, 0, 0, blueLed);
        }
        for (int j = (perChannel * 3); j < PIXEL_COUNT; j++) {
            neopixel_set_color(j, 0, 0, blueLed);
        }
    } else if (neopixelSettings.neopixelMode == 2) {
        for (uint16_t i = 0; i < PIXEL_COUNT; i++) {
            neopixel_set_color(i, redLed, 0, 0);
        }
    } else if (neopixelSettings.neopixelMode == 3) {
        float NLEDS = ((float)r * PIXEL_COUNT); // light up this many leds
        int ledsFully = (int)floorf(NLEDS);
        int ledsNext = (int)ceilf(NLEDS);
        int ledTransitionBrightness = (int)((NLEDS - (float)ledsFully) * (float)neopixelSettings.maxRedBrightness);
        for (int j = 0; j < PIXEL_COUNT; j++) {
            if (j < ledsFully) {
                neopixel_set_color(j, neopixelSettings.maxRedBrightness, 0, 0);
            } else {
                if (j < ledsNext) {
                    neopixel_set_color(j, ledTransitionBrightness, 0, 0);
                } else {
                    neopixel_set_color(j, 0, 0, 0);
                }
            }
        }
    } else if (neopixelSettings.neopixelMode == 4) {
        float NLEDS = ((float)g * PIXEL_COUNT); // light up this many leds
        int ledsFully = (int)floorf(NLEDS);
        int ledsNext = (int)ceilf(NLEDS);
        int ledTransitionBrightness = (int)((NLEDS - (float)ledsFully) * (float)neopixelSettings.maxGreenBrightness);
        for (int j = 0; j < PIXEL_COUNT; j++) {
            if (j < ledsFully) {
                neopixel_set_color(j, 0, neopixelSettings.maxGreenBrightness, 0);
            } else {
                if (j < ledsNext) {
                    neopixel_set_color(j, 0, ledTransitionBrightness, 0);
                } else {
                    neopixel_set_color(j, 0, 0, 0);
                }
            }
        }
    } else if (neopixelSettings.neopixelMode == 5) {
        float NLEDS = ((float)b * PIXEL_COUNT); // light up this many leds
        int ledsFully = (int)floorf(NLEDS);
        int ledsNext = (int)ceilf(NLEDS);
        int ledTransitionBrightness = (int)((NLEDS - (float)ledsFully) * (float)neopixelSettings.maxBlueBrightness);
        for (int j = 0; j < PIXEL_COUNT; j++) {
            if (j < ledsFully) {
                neopixel_set_color(j, 0, 0, neopixelSettings.maxBlueBrightness);
            } else {
                if (j < ledsNext) {
                    neopixel_set_color(j, 0, 0, ledTransitionBrightness);
                } else {
                    neopixel_set_color(j, 0, 0, 0);
                }
            }
        }
    } else if (neopixelSettings.neopixelMode == 6) {
        // Light up this many leds
        float NLEDSR = (r * (float)PIXEL_COUNT);
        float NLEDSG = (g * (float)PIXEL_COUNT);
        float NLEDSB = (b * (float)PIXEL_COUNT);
        // Calculate red transition
        int ledsFullyR = (int)floorf(NLEDSR);
        int ledsNextR = (int)ceilf(NLEDSR);
        int ledTransitionBrightnessR = (int)((NLEDSR - (float)ledsFullyR) * (float)neopixelSettings.maxRedBrightness);
        // Calculate green transition
        int ledsFullyG = (int)floorf(NLEDSG);
        int ledsNextG = (int)ceilf(NLEDSG);
        int ledTransitionBrightnessG = (int)((NLEDSG - (float)ledsFullyG) * (float)neopixelSettings.maxGreenBrightness);
        // Calculate blue transition
        int ledsFullyB = (int)floorf(NLEDSB);
        int ledsNextB = (int)ceilf(NLEDSB);
        int ledTransitionBrightnessB = (int)((NLEDSB - (float)ledsFullyB) * (float)neopixelSettings.maxBlueBrightness);
        // Display
        for (int j = 0; j < PIXEL_COUNT; j++) {
            uint8_t currentR = 0;
            uint8_t currentG = 0;
            uint8_t currentB = 0;
            // Red
            if (j < ledsFullyR) {
                currentR = neopixelSettings.maxRedBrightness;
            } else {
                if (j < ledsNextR) {
                    currentR = ledTransitionBrightnessR;
                } else {
                    currentR = 0;
                }
            }
            // Green
            if (j < ledsFullyG) {
                currentG = neopixelSettings.maxGreenBrightness;
            } else {
                if (j < ledsNextG) {
                    currentG = ledTransitionBrightnessG;
                } else {
                    currentG = 0;
                }
            }
            // Blue
            if (j < ledsFullyB) {
                currentB = neopixelSettings.maxBlueBrightness;
            } else {
                if (j < ledsNextB) {
                    currentB = ledTransitionBrightnessB;
                } else {
                    currentB = 0;
                }
            }
            // Set
            neopixel_set_color(j, currentR, currentG, currentB);
        }
    }
    neopixel_update();
}

neopixel_settings_s neopixel_get_settings() {
    return neopixelSettings;
}

void neopixel_set_settings(neopixel_settings_s new_settings) {
    // Verify mode
    if (new_settings.neopixelMode > 6) new_settings.neopixelMode = 6;
    if (new_settings.neopixelMode < 0) new_settings.neopixelMode = 0;

    // Verify brightness min
    if (new_settings.maxRedBrightness < 0) new_settings.maxRedBrightness = 0;
    if (new_settings.maxGreenBrightness < 0) new_settings.maxGreenBrightness = 0;
    if (new_settings.maxBlueBrightness < 0) new_settings.maxBlueBrightness = 0;

    // Verify brightness max
    if (new_settings.maxRedBrightness > 255) new_settings.maxRedBrightness = 255;
    if (new_settings.maxGreenBrightness > 255) new_settings.maxGreenBrightness = 255;
    if (new_settings.maxBlueBrightness > 255) new_settings.maxBlueBrightness = 255;

    neopixelSettings = new_settings;
}