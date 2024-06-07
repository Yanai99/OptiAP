#include "rgb_led.h"

led_strip_handle_t led;

void configure_led(void)
{
    led_strip_config_t strip_config = {
        .strip_gpio_num = CONFIG_RGB_LED_GPIO,
        .max_leds = 1,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB,
        .led_model = LED_MODEL_WS2812,
        .flags.invert_out = false
    };

    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10 * 1000 * 1000, // control the speed of the LED strip - the
        .flags.with_dma = false
    };

    // LED Strip object handle
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led));
}

void set_led_color_with_brightness(uint32_t red, uint32_t green, uint32_t blue, uint32_t brightness)
{
    ESP_ERROR_CHECK(led_strip_set_pixel(led, 0, red * brightness / 255, green * brightness / 255, blue * brightness / 255));
    ESP_ERROR_CHECK(led_strip_refresh(led));
}

void clear_led(void)
{
    ESP_ERROR_CHECK(led_strip_clear(led));
    ESP_ERROR_CHECK(led_strip_refresh(led));
}