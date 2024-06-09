#pragma once

#include "led_strip.h"

#include "common.h"

typedef struct {
    uint32_t red;
    uint32_t green;
    uint32_t blue;
    uint32_t brightness;
    uint32_t delay;
} rgb_brightness_delay_t;

extern led_strip_handle_t led;

#define LED_BRIGHTNESS 10

/**
 * @brief Configure the RGB LED
 * 
 * @attention This function must be called before any other function in this file
 * 
 * @return void
 */
void configure_led(void);

/**
 * @brief Set the color of the RGB LED
 * 
 * @param red Red value (0-255)
 * @param green Green value (0-255)
 * @param blue Blue value (0-255)
 * @param brightness Brightness value (0-255)
 * 
 * @return void
 */
void set_led_color_with_brightness(uint32_t red, uint32_t green, uint32_t blue, uint32_t brightness);

void clear_led(void);

void flash_led(void *pvRgbBrightnessDelay);