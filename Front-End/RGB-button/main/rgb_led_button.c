#include <stdio.h>

#include "esp_err.h"
#include "esp_log.h"

#include "led_strip.h"
#include "iot_button.h"

#define RGB_LED_GPIO 48
#define BUTTON_GPIO 0 // BOOT button

const char *TAG_BUTTON = "button";
const char *TAG_LED = "led";

esp_err_t init_led_strip(led_strip_handle_t *strip)
{
    /* LED strip initialization with the GPIO and pixels number*/
    led_strip_config_t strip_config = {
        .strip_gpio_num = RGB_LED_GPIO, // The GPIO that connected to the LED strip's data line
        .max_leds = 1, // The number of LEDs in the strip,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB, // Pixel format of your LED strip
        .led_model = LED_MODEL_WS2812, // LED strip model
        .flags.invert_out = false, // whether to invert the output signal (useful when your hardware has a level inverter)
    };

    /* rmt configuration */
    led_strip_rmt_config_t rmt_config = {
    #if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 0, 0)
        .rmt_channel = 0,
    #else
        .clk_src = RMT_CLK_SRC_DEFAULT, // different clock source can lead to different power consumption
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
        .flags.with_dma = false, // whether to enable the DMA feature
    #endif
    };

    /* Create a new LED strip with the configuration */
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, strip));

    /* Clear the LED strip */
    ESP_ERROR_CHECK(led_strip_clear(*strip));

    ESP_LOGI(TAG_LED, "LED strip initialized");

    return ESP_OK;
}

esp_err_t init_button(button_handle_t *btn)
{
    /* Button configuration */
    button_config_t button_config = {
        .type = BUTTON_TYPE_GPIO,
        .long_press_time = 0,
        .short_press_time = 0,
        .gpio_button_config = {
            .gpio_num = BUTTON_GPIO,
            .active_level = 0
        }
    };

    /* Create a new button with the configuration */
    *btn = iot_button_create(&button_config);
    if (*btn == NULL)
    {
        ESP_LOGE(TAG_BUTTON, "Failed to create button");
        return ESP_FAIL;
    }

    return ESP_OK;
}

void button_single_press_cb(void *arg, void *strip)
{
    ESP_LOGI(TAG_BUTTON, "Button single press");
    ESP_ERROR_CHECK(led_strip_set_pixel(strip, 0, 0, 255, 0)); // set the LED to green
    ESP_ERROR_CHECK(led_strip_refresh(strip));
}

void button_double_press_cb(void *arg, void *strip)
{
    ESP_LOGI(TAG_BUTTON, "Button double press");
    ESP_ERROR_CHECK(led_strip_set_pixel(strip, 0, 255, 0, 0)); // set the LED to red
    ESP_ERROR_CHECK(led_strip_refresh(strip));
}

void button_long_press_cb(void *arg, void *strip)
{
    ESP_LOGI(TAG_BUTTON, "Button long press");
    ESP_ERROR_CHECK(led_strip_clear(strip)); // clear the LED strip
    ESP_ERROR_CHECK(led_strip_refresh(strip));
}

esp_err_t assign_rgb_functionalities_to_button(button_handle_t *btn, led_strip_handle_t strip)
{
    ESP_ERROR_CHECK(iot_button_register_cb(*btn, BUTTON_SINGLE_CLICK, button_single_press_cb, strip));
    ESP_ERROR_CHECK(iot_button_register_cb(*btn, BUTTON_DOUBLE_CLICK, button_double_press_cb, strip));
    ESP_ERROR_CHECK(iot_button_register_cb(*btn, BUTTON_LONG_PRESS_START, button_long_press_cb, strip));

    return ESP_OK;
}

void app_main(void)
{
    led_strip_handle_t strip;
    ESP_ERROR_CHECK(init_led_strip(&strip));

    button_handle_t btn;
    ESP_ERROR_CHECK(init_button(&btn));
    ESP_ERROR_CHECK(assign_rgb_functionalities_to_button(&btn, strip));
    
    while (true)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay 1 second
    }
}