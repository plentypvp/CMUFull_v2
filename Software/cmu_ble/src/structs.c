#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    uint8_t neopixelMode; // 0 - 3
    uint8_t maxRedBrightness; // 0 - 255
    uint8_t maxGreenBrightness; // 0 - 255
    uint8_t maxBlueBrightness; // 0 - 255
} neopixel_settings_s;

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

neopixel_settings_s get_neopixel_settings(uint8_t* bytes) {
    neopixel_settings_s res = {};
    memcpy(&res, bytes, sizeof(neopixel_settings_s));
    return res;
}

dsp_settings_s get_dsp_settings(uint8_t* bytes) {
    dsp_settings_s res = {};
    memcpy(&res, bytes, sizeof(dsp_settings_s));
    return res;
}

uint8_t* set_neopixel_settings(neopixel_settings_s structure) {
    static uint8_t res[sizeof(neopixel_settings_s)];
    memcpy(res, &structure, sizeof(neopixel_settings_s));
    return res;
}

uint8_t* set_dsp_settings(dsp_settings_s structure) {
    static uint8_t res[sizeof(dsp_settings_s)];
    memcpy(res, &structure, sizeof(dsp_settings_s));
    return res;
}