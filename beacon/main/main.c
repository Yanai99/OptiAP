#include "wifi.h"
#include "nvs_flash.h"

void app_main(void)
{
    // nvs
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    esp_netif_t *ap_netif = wifi_init();

    while (1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
