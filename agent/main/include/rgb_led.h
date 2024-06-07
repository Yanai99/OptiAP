#pragma once

#include "led_strip.h"

extern led_strip_handle_t led;

#define LED_BRIGHTNESS 10

void configure_led(void);

void set_led_color_with_brightness(uint32_t red, uint32_t green, uint32_t blue, uint32_t brightness);

void clear_led(void);