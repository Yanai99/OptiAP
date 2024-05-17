#include "wifi.h"

esp_netif_t* wifi_init(void)
{
    // Set the WiFi mode to AP
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_t *ap_netif = esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Set the WiFi configuration
    wifi_config_t wifi_config = 
    {
        .ap.ssid = WIFI_SSID,
        .ap.ssid_len = strlen(WIFI_SSID),
        .ap.password = WIFI_PASS,
        .ap.max_connection = 1,
        .ap.authmode = WIFI_AUTH_WPA2_PSK,
        .ap.ftm_responder = true,
        .ap.ssid_hidden = true
    };

    if (strlen(WIFI_PASS) == 0)
    {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_set_bandwidth(ESP_IF_WIFI_AP, WIFI_BW_HT20));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG_WIFI, "AP WiFi started");

    return ap_netif;
}