#include <stdio.h>

#include "nvs_flash.h"
#include "esp_log.h"

#include "wifi.h"
#include "button.h"
#include "rgb_led.h"

void app_main(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // clear LED
    configure_led();
    clear_led();

    // orange color
    set_led_color_with_brightness(255, 165, 0, LED_BRIGHTNESS);
    
    // Initialize WiFi
    esp_netif_t *sta_netif = NULL;
    esp_netif_t *ap_netif = NULL;

    if (wifi_init(sta_netif, ap_netif) != ESP_OK)
    {
        ESP_LOGE(TAG_WIFI, "Failed to initialize Wi-Fi as station");
        return;
    }
    
    button_handle_t btn;
    ESP_ERROR_CHECK(init_button(&btn));
    ESP_ERROR_CHECK(assign_functionalities_to_button(&btn, ftm_procedure, NULL, NULL));

    set_led_color_with_brightness(0, 255, 0, LED_BRIGHTNESS);

    // loop
    while (1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}