#include <stdio.h>
#include <string.h>

#include "esp_wifi.h"
#include "esp_mac.h"

#include "cmd_system.h"
#include "esp_err.h"
#include "esp_log.h"
#include "nvs_flash.h"

#define ETH_ALEN 6
#define MAX_CONNECT_RETRY_ATTEMPTS  5
#define DEFAULT_WAIT_TIME_MS        (10 * 1000) // 10 seconds

static bool s_reconnect = true;
static uint8_t s_ap_channel;
static uint8_t s_ap_bssid[ETH_ALEN];
static int s_retry_num = 0;
static uint32_t s_rtt_est, s_dist_est;
static uint8_t s_ftm_report_num_entries;
static bool s_ap_started;

static const char *TAG_AP = "ftm_ap";
static const char *TAG_STA = "ftm_sta";

static EventGroupHandle_t s_wifi_event_group;
static const int CONNECTED_BIT = BIT0;
static const int DISCONNECTED_BIT = BIT1;

static EventGroupHandle_t s_ftm_event_group;
static const int FTM_REPORT_BIT = BIT0;
static const int FTM_FAILURE_BIT = BIT1;

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
	if (event_id == WIFI_EVENT_STA_CONNECTED) {
        wifi_event_sta_connected_t *event = (wifi_event_sta_connected_t *)event_data;

        ESP_LOGI(TAG_STA, "Connected to %s (BSSID: "MACSTR", Channel: %d)", event->ssid,
                 MAC2STR(event->bssid), event->channel);

        memcpy(s_ap_bssid, event->bssid, ETH_ALEN);
        s_ap_channel = event->channel;
        xEventGroupClearBits(s_wifi_event_group, DISCONNECTED_BIT);
        xEventGroupSetBits(s_wifi_event_group, CONNECTED_BIT);
    } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_reconnect && ++s_retry_num < MAX_CONNECT_RETRY_ATTEMPTS) {
            ESP_LOGI(TAG_STA, "sta disconnect, retry attempt %d...", s_retry_num);
            esp_wifi_connect();
        } else {
            ESP_LOGI(TAG_STA, "sta disconnected");
        }
        xEventGroupClearBits(s_wifi_event_group, CONNECTED_BIT);
        xEventGroupSetBits(s_wifi_event_group, DISCONNECTED_BIT);
    } else if (event_id == WIFI_EVENT_FTM_REPORT) {
        wifi_event_ftm_report_t *event = (wifi_event_ftm_report_t *) event_data;

        s_rtt_est = event->rtt_est;
        s_dist_est = event->dist_est;
        s_ftm_report_num_entries = event->ftm_report_num_entries;
        if (event->status == FTM_STATUS_SUCCESS) {
            xEventGroupSetBits(s_ftm_event_group, FTM_REPORT_BIT);
        } else if (event->status == FTM_STATUS_USER_TERM) {
            /* Do Nothing */
            ESP_LOGI(TAG_STA, "User terminated FTM procedure");
        } else {
            ESP_LOGI(TAG_STA, "FTM procedure with Peer("MACSTR") failed! (Status - %d)",
                     MAC2STR(event->peer_mac), event->status);
            xEventGroupSetBits(s_ftm_event_group, FTM_FAILURE_BIT);
        }
    } else if (event_id == WIFI_EVENT_AP_START) {
        s_ap_started = true;
    } else if (event_id == WIFI_EVENT_AP_STOP) {
        s_ap_started = false;
    }
}

void initialise_wifi(void)
{
    esp_log_level_set("wifi", ESP_LOG_WARN);
    static bool initialized = false;

    if (initialized) {
        return;
    }

    ESP_ERROR_CHECK(esp_netif_init());
    s_wifi_event_group = xEventGroupCreate();
    s_ftm_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL) );
    ESP_ERROR_CHECK(esp_wifi_start() );
    initialized = true;
}

void register_wifi(void)
{
    /* AP Configuration */
    wifi_config_t g_ap_config = {
    .ap.authmode = WIFI_AUTH_WPA2_PSK,
    .ap.ftm_responder = true,
    .ap.ssid = "FTM responder",
    .ap.password = "12345678",
    .ap.channel = 1,
    .ap.beacon_interval = 100
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &g_ap_config));
}
void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    initialise_wifi();

    register_system();
    register_wifi();

    while (1) {
        vTaskDelay(DEFAULT_WAIT_TIME_MS / portTICK_PERIOD_MS);
    }
}