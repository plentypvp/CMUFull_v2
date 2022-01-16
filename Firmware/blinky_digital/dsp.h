#include "neopixel.h"
#include <nrfx_saadc.h>
// FPU stuff
/*lint -save -e689 */ /* Apparent end of comment ignored */
#include "arm_const_structs.h"
/*lint -restore */

#ifndef BLINKY_DIGITAL_DSP_H
#define BLINKY_DIGITAL_DSP_H

// FPU stuff
#define FPU_EXCEPTION_MASK 0x0000009F
#define FPU_FPSCR_REG_STACK_OFF 0x40

// IIR filters
#define IIR_ORDER 6
#define IIR_NUMSTAGES (IIR_ORDER/2)

// Sampling
#define SAMPLE_BUF_SIZE 1250
#define SAADC_BUF_COUNT 2
#define SAADC_SAMPLE_FREQUENCY 31250
#define INTEGRATOR_SIZE 5

typedef struct {
    uint8_t redIntegrator;
    uint8_t greenIntegrator;
    uint8_t blueIntegrator;
    uint16_t redMinAmplitude;
    uint16_t redMaxAmplitude;
    uint16_t greenMinAmplitude;
    uint16_t greenMaxAmplitude;
    uint16_t blueMinAmplitude;
    uint16_t blueMaxAmplitude;
} dsp_settings_s;

void saadc_init(void);
dsp_settings_s dsp_get_settings();
void dsp_set_settings(dsp_settings_s new_settings);

#endif //BLINKY_DIGITAL_DSP_H
