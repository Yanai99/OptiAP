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

static const char *TAG_STA = "ftm_station";

static EventGroupHandle_t s_wifi_event_group;
static const int CONNECTED_BIT = BIT0;
static const int DISCONNECTED_BIT = BIT1;

static EventGroupHandle_t s_ftm_event_group;
static const int FTM_REPORT_BIT = BIT0;
static const int FTM_FAILURE_BIT = BIT1;

typedef struct
{
    uint8_t mac[ETH_ALEN];
    uint8_t channel;
    char ssid[32];
} wifi_ap_ftm;

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
	if (event_id == WIFI_EVENT_STA_CONNECTED)
    {
        wifi_event_sta_connected_t *event = (wifi_event_sta_connected_t *)event_data;

        ESP_LOGI(TAG_STA, "Connected to %s (BSSID: "MACSTR", Channel: %d)", event->ssid,
                 MAC2STR(event->bssid), event->channel);

        memcpy(s_ap_bssid, event->bssid, ETH_ALEN);
        s_ap_channel = event->channel;
        xEventGroupClearBits(s_wifi_event_group, DISCONNECTED_BIT);
        xEventGroupSetBits(s_wifi_event_group, CONNECTED_BIT);
    }
    else if (event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_reconnect && ++s_retry_num < MAX_CONNECT_RETRY_ATTEMPTS) {
            ESP_LOGI(TAG_STA, "sta disconnect, retry attempt %d...", s_retry_num);
            esp_wifi_connect();
        } else {
            ESP_LOGI(TAG_STA, "sta disconnected");
        }
        xEventGroupClearBits(s_wifi_event_group, CONNECTED_BIT);
        xEventGroupSetBits(s_wifi_event_group, DISCONNECTED_BIT);
    }
    else if (event_id == WIFI_EVENT_FTM_REPORT)
    {
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
    }
    else if (event_id == WIFI_EVENT_AP_START)
    {
        s_ap_started = true;
    }
    else if (event_id == WIFI_EVENT_AP_STOP)
    {
        s_ap_started = false;
    }
}

void initialise_wifi(void)
{
    esp_log_level_set("wifi", ESP_LOG_WARN);
    static bool initialized = false;

    if (initialized)
    {
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
    /* STA Configuration */
    wifi_scan_config_t scan_config = {
        .ssid = NULL,
        .bssid = NULL,
        .channel = 0,
        .show_hidden = true,
        .scan_type = WIFI_SCAN_TYPE_ACTIVE,
        .scan_time.active.min = 0,
        .scan_time.active.max = 200
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true));
}

void scan_aps(wifi_ap_ftm *ftm_aps, uint16_t *num_ftm_aps, bool print)
{
    uint16_t ap_num = 0;
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_num));
    if (ap_num == 0) // we want to return num_ftm_aps = 0 if there are no APs, and not allocate memory for ftm_aps
    {
        *num_ftm_aps = 0;
        return;
    }

    wifi_ap_record_t *ap_list = (wifi_ap_record_t *)malloc(ap_num * sizeof(wifi_ap_record_t));
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&ap_num, ap_list));

    for (int i = 0; i < ap_num; i++)
    {
        if (ap_list[i].ftm_responder)
        {
            memcpy(ftm_aps[*num_ftm_aps].mac, ap_list[i].bssid, ETH_ALEN);
            ftm_aps[*num_ftm_aps].channel = ap_list[i].primary;
            memcpy(ftm_aps[*num_ftm_aps].ssid, ap_list[i].ssid, 32);
            (*num_ftm_aps)++;
        }
        if (print)
        {
            // log available data: MAC, channel, ssid, rssi, etc.
            ESP_LOGI(TAG_STA, "SSID: %s, RSSI: %d, supports FTM: %s, MAC of AP: "MACSTR,
                        (char *)ap_list[i].ssid, ap_list[i].rssi,
                        ap_list[i].ftm_responder ? "Yes" : "No",
                        MAC2STR(ap_list[i].bssid));
        }
    }

    free(ap_list);
}

/**
  * @brief      Find the FTM responder AP and save its MAC address and channel.
  *
  * @attention  This function uses the scan_aps function to find all APs that support FTM.
  *             It then searches for the AP with the SSID that starts with "FTM responder" and saves its MAC address and channel.
  * 
  * @attention  This function assumes that there is only one AP with the SSID "FTM responder" that supports FTM.
  * 
  * @attention  This function uses the global variables s_ap_bssid and s_ap_channel to store the MAC address and channel of the FTM responder.
  *
  * @param      log_ap_scan If true, the function logs the activation of scan_aps
  * 
  * @return
  *    - true: FTM responder found
  *    - false: FTM responder not found
  */
bool find_ftm_ap(bool log_ap_scan)
{
    // we want to use the scan_aps function to find all APs that support FTM. For that, we prepare here an empty list that can store mac addresses and channels of all APs that support FTM
    wifi_ap_ftm *ftm_aps = (wifi_ap_ftm *)malloc(100 * sizeof(wifi_ap_ftm));
    uint16_t num_ftm_aps = 0;

    scan_aps(ftm_aps, &num_ftm_aps, log_ap_scan);

    if (num_ftm_aps == 0)
    {
        ESP_LOGI(TAG_STA, "No APs found that support FTM");
        return false;
    }
    else
    {
        ESP_LOGI(TAG_STA, "Found %d APs that support FTM", num_ftm_aps);
    }

    // we want to print the MAC addresses and channels of the access point that its ssid starts with "FTM responder". There can be only one such AP. Also 
    for (int i = 0; i < num_ftm_aps; i++)
    {
        if (strncmp(ftm_aps[i].ssid, "FTM responder", 13) == 0)
        {
            ESP_LOGI(TAG_STA, "FTM responder found: MAC: "MACSTR", Channel: %d", MAC2STR(ftm_aps[i].mac), ftm_aps[i].channel);
            // copy the MAC address and channel of the FTM responder to the global variables
            memcpy(s_ap_bssid, ftm_aps[i].mac, ETH_ALEN);
            s_ap_channel = ftm_aps[i].channel;
            break;
        }
    }

    // free the memory
    free(ftm_aps);

    return true;
}

void ftm_procedure(void)
{
    wifi_ftm_initiator_cfg_t ftm_initiator_cfg = { // FTM initiator configuration
        .resp_mac = {s_ap_bssid[0], s_ap_bssid[1], s_ap_bssid[2], s_ap_bssid[3], s_ap_bssid[4], s_ap_bssid[5]},
        .channel = s_ap_channel,
        .frm_count = 0,
        .burst_period = 0,
        .use_get_report_api = true
    };

    ESP_ERROR_CHECK(esp_wifi_ftm_initiate_session(&ftm_initiator_cfg));

    EventBits_t bits = xEventGroupWaitBits(s_ftm_event_group, FTM_REPORT_BIT | FTM_FAILURE_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
    if (bits & FTM_REPORT_BIT) {
        ESP_LOGI(TAG_STA, "FTM Report received successfully!");
    } else if (bits & FTM_FAILURE_BIT) {
        ESP_LOGI(TAG_STA, "FTM Report failed!");
        return;
    }

    // print the FTM report
    wifi_ftm_report_entry_t *ftm_report = (wifi_ftm_report_entry_t *)malloc(sizeof(wifi_ftm_report_entry_t) * s_ftm_report_num_entries);
    ESP_ERROR_CHECK(esp_wifi_ftm_get_report(ftm_report, s_ftm_report_num_entries));

    ESP_LOGI(TAG_STA, "Estimated RTT - %" PRId32 " nSec, Estimated Distance - %" PRId32 ".%02" PRId32 " meters",
            s_rtt_est, s_dist_est / 100, s_dist_est % 100);

    // free the memory
    free(ftm_report);
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

    bool log_ap_scan = true;
    find_ftm_ap(log_ap_scan);

    while (1) {
        vTaskDelay(DEFAULT_WAIT_TIME_MS / portTICK_PERIOD_MS);
        ftm_procedure();
    }
}