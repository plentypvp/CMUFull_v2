#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "nrfx_pwm.h"
#include "nrf_delay.h"

#ifndef BLINKY_NEOPIXEL_H
#define BLINKY_NEOPIXEL_H

#define PIXEL_COUNT 60
#define BITS_PER_PIXEL 24
#define NEOPIXEL_PIN 6

#define NEOPIXEL_INITIAL_MODE 1

typedef struct {
    uint8_t neopixelMode; // 0 - 3
    uint8_t maxRedBrightness; // 0 - 255
    uint8_t maxGreenBrightness; // 0 - 255
    uint8_t maxBlueBrightness; // 0 - 255
} neopixel_settings_s;

void neopixel_init();
void neopixel_set_color(uint8_t i, uint8_t r, uint8_t g, uint8_t b);
void neopixel_sequence_handler(nrfx_pwm_evt_type_t event_type);
void neopixel_update();
void neopixel_colorwipe(uint8_t r, uint8_t g, uint8_t b);
void neopixel_set_cmu_color(float r, float g, float b);
void neopixel_set_settings(neopixel_settings_s new_settings);
neopixel_settings_s neopixel_get_settings();

#endif //BLINKY_NEOPIXEL_H
