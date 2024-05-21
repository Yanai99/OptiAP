#include "wifi.h"

EventGroupHandle_t ftm_event_group;

uint32_t rtt_raw;
uint32_t rtt_est;
uint32_t dist_est;

esp_ip4_addr_t server_ip = {0};

esp_http_client_config_t config = {
    .host = "192.168.4.2",
    .port = 8080,
    .path = "store-data",
    .method = HTTP_METHOD_POST,
    .event_handler = http_event_handler,
    .keep_alive_enable = true,
    .keep_alive_idle = 10,
    .keep_alive_interval = 10,
    .keep_alive_count = 3
};

esp_http_client_handle_t client;

void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT) {
        if (event_id == WIFI_EVENT_WIFI_READY) {
            ESP_LOGI(TAG_WIFI, "WiFi ready");
        } else if (event_id == WIFI_EVENT_SCAN_DONE) {
            // ESP_LOGI(TAG_WIFI, "Scan done");
        } else if (event_id == WIFI_EVENT_STA_START) {
            ESP_LOGI(TAG_WIFI, "Station start");
        } else if (event_id == WIFI_EVENT_STA_STOP) {
            ESP_LOGI(TAG_WIFI, "Station stop");
        } else if (event_id == WIFI_EVENT_STA_CONNECTED) {
            ESP_LOGI(TAG_WIFI, "Station connected to AP");
        } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
            ESP_LOGI(TAG_WIFI, "Station disconnected from AP");
        } else if (event_id == WIFI_EVENT_STA_AUTHMODE_CHANGE) {
            ESP_LOGI(TAG_WIFI, "Auth mode of AP connected by device's station changed");
        } else if (event_id == WIFI_EVENT_AP_START) {
            ESP_LOGI(TAG_WIFI, "Soft-AP start");
        } else if (event_id == WIFI_EVENT_AP_STOP) {
            ESP_LOGI(TAG_WIFI, "Soft-AP stop");
        } else if (event_id == WIFI_EVENT_AP_STACONNECTED) {
            ESP_LOGI(TAG_WIFI, "A station connected to Soft-AP");
            wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *) event_data;
            ESP_LOGI(TAG_WIFI, "Station connected: ["MACSTR"] aid: [%d]",
                MAC2STR(event->mac), event->aid);
        } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
            ESP_LOGI(TAG_WIFI, "A station disconnected from Soft-AP");
            wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *) event_data;
            ESP_LOGI(TAG_WIFI, "Station disconnected: ["MACSTR"] aid: [%d]",
                MAC2STR(event->mac), event->aid);
        } else if (event_id == WIFI_EVENT_AP_PROBEREQRECVED) {
            ESP_LOGI(TAG_WIFI, "Receive probe request packet in Soft-AP interface");
        } else if (event_id == WIFI_EVENT_FTM_REPORT) {
            ESP_LOGI(TAG_WIFI, "Receive report of FTM procedure"); 
            wifi_event_ftm_report_t *event = (wifi_event_ftm_report_t *) event_data;
            if (event->status == FTM_STATUS_SUCCESS) {
                ESP_LOGI(TAG_WIFI, "FTM report success");
                rtt_raw = event->rtt_raw;
                rtt_est = event->rtt_est;
                dist_est = event->dist_est;
                xEventGroupSetBits(ftm_event_group, FTM_SUCCESS_BIT);
            } else {
                ESP_LOGI(TAG_WIFI, "FTM procedure with Peer ["MACSTR"] failed! Status [%d]",
                        MAC2STR(event->peer_mac), event->status);
                xEventGroupSetBits(ftm_event_group, FTM_FAILURE_BIT);
            }
        } else if (event_id == WIFI_EVENT_STA_BSS_RSSI_LOW) {
            ESP_LOGI(TAG_WIFI, "AP's RSSI crossed configured threshold");
        } else if (event_id == WIFI_EVENT_ACTION_TX_STATUS) {
            ESP_LOGI(TAG_WIFI, "Status indication of Action Tx operation");
        } else if (event_id == WIFI_EVENT_STA_BEACON_TIMEOUT) {
            ESP_LOGI(TAG_WIFI, "Station beacon timeout");
        } else if (event_id == WIFI_EVENT_CONNECTIONLESS_MODULE_WAKE_INTERVAL_START) {
            ESP_LOGI(TAG_WIFI, "Connectionless module wake interval start");
        } else if (event_id == WIFI_EVENT_HOME_CHANNEL_CHANGE) {
            ESP_LOGI(TAG_WIFI, "WiFi home channel change, doesn't occur when scanning");
        } else {
            ESP_LOGI(TAG_WIFI, "Unknown event");
        }
    }
}

void ip_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == IP_EVENT) {
        if (event_id == IP_EVENT_STA_GOT_IP) {
            ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
            ESP_LOGI(TAG_WIFI, "Got IP [" IPSTR"]", IP2STR(&event->ip_info.ip));
        } else if (event_id == IP_EVENT_STA_LOST_IP) {
            ESP_LOGI(TAG_WIFI, "Lost IP");
            server_ip.addr = 0;
        } else if (event_id == IP_EVENT_AP_STAIPASSIGNED) {
            ESP_LOGI(TAG_WIFI, "Assigned IP to station");
            ip_event_ap_staipassigned_t* event = (ip_event_ap_staipassigned_t*) event_data;
            ESP_LOGI(TAG_WIFI, "Assigned IP [" IPSTR "] to MAC [" MACSTR"]", IP2STR(&event->ip), MAC2STR(event->mac));
            server_ip.addr = event->ip.addr;
        } else {
            ESP_LOGI(TAG_WIFI, "Unknown event");
        }
    }
}

esp_err_t http_event_handler(esp_http_client_event_t *event)
{
    switch(event->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG_WIFI, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG_WIFI, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG_WIFI, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG_WIFI, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", event->header_key, event->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG_WIFI, "HTTP_EVENT_ON_DATA, len=%d", event->data_len);
            char *buf = (char *)malloc(event->data_len + 1);
            memcpy(buf, event->data, event->data_len);
            buf[event->data_len] = '\0';
            ESP_LOGI(TAG_WIFI, "Response: %s", buf);
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG_WIFI, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG_WIFI, "HTTP_EVENT_DISCONNECTED");
            break;
        case HTTP_EVENT_REDIRECT:
            ESP_LOGI(TAG_WIFI, "HTTP_EVENT_REDIRECT");
            break;
    }
    return ESP_OK;
}

esp_err_t wifi_init(esp_netif_t* sta_netif, esp_netif_t* ap_netif)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ap_netif = esp_netif_create_default_wifi_ap();
    if (ap_netif == NULL) {
        return ESP_ERR_NO_MEM;
    }

    ftm_event_group = xEventGroupCreate();
    if (ftm_event_group == NULL) {
        return ESP_ERR_NO_MEM;
    }

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &ip_event_handler, NULL));

    wifi_config_t wifi_config =  {
        .ap.ssid = CONFIG_WIFI_SSID,
        .ap.ssid_len = strlen(CONFIG_WIFI_SSID),
        .ap.password = CONFIG_WIFI_PASSWORD,
        .ap.max_connection = 3,
        .ap.authmode = WIFI_AUTH_WPA2_PSK,
        .ap.ssid_hidden = false
    };

    if (strlen(CONFIG_WIFI_PASSWORD) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_set_bandwidth(ESP_IF_WIFI_STA, WIFI_BW_HT20));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG_WIFI, "Wi-Fi started");

    // we want to connect to an AP
    wifi_config_t sta_config = {
        .sta = {
            .ssid = CONFIG_WIFI_SSID,
            .password = CONFIG_WIFI_PASSWORD
        }
    };

    if (strlen(CONFIG_WIFI_PASSWORD) == 0) {
        sta_config.sta.threshold.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &sta_config));
    ESP_ERROR_CHECK(esp_wifi_connect());

    client = esp_http_client_init(&config);

    return ESP_OK;
}

void scan_ftm_responders(int* num_ftm_responders, ftm_responder_t* ftm_responders)
{
    wifi_scan_config_t scan_config = {
        .scan_type = WIFI_SCAN_TYPE_ACTIVE,
        .ssid = NULL,
        .bssid = NULL,
        .scan_time.active.min = 0,
        .scan_time.active.max = 120,
        .show_hidden = true
    };

    ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true));
    uint16_t ap_num = 0;
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_num));
    wifi_ap_record_t *ap_records = (wifi_ap_record_t *)malloc(sizeof(wifi_ap_record_t) * ap_num);
    esp_wifi_scan_get_ap_records(&ap_num, ap_records);

    *num_ftm_responders = 0;
    for (int i = 0; i < ap_num; i++) {
        if (ap_records[i].ftm_responder) {
            memcpy(ftm_responders[*num_ftm_responders].mac, ap_records[i].bssid, 6);
            ftm_responders[*num_ftm_responders].channel = ap_records[i].primary;
            ftm_responders[*num_ftm_responders].rssi = ap_records[i].rssi;
            (*num_ftm_responders)++;
        }
        ESP_LOGI(TAG_WIFI, "AP ["MACSTR"] SSID [%s] RSSI [%d] Channel [%d] FTM responder [%d]",
                MAC2STR(ap_records[i].bssid), (char *)ap_records[i].ssid, ap_records[i].rssi, ap_records[i].primary, ap_records[i].ftm_responder);
    }

    free(ap_records);
}

esp_err_t http_post_data(char *post_data)
{
    ESP_ERROR_CHECK(esp_http_client_set_header(client, "Content-Type", "application/json"));
    //ESP_ERROR_CHECK(esp_http_client_set_header(client, "Connection", "keep-alive"));
    ESP_ERROR_CHECK(esp_http_client_set_post_field(client, post_data, strlen(post_data)));

    esp_http_client_set_method(client, HTTP_METHOD_POST);

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG_WIFI, "HTTP POST Status = %d",
                esp_http_client_get_status_code(client));
        
    } else {
        ESP_LOGE(TAG_WIFI, "HTTP POST request failed: %s", esp_err_to_name(err));
    }

    return err;
}

void ftm_procedure(void *btn_plus_task_to_create)
{
    int num_ftm_responders = 0;
    ftm_responder_t ftm_responders[MAX_FTM_RESPONDERS];
    scan_ftm_responders(&num_ftm_responders, ftm_responders);

    button_task_t *btn_plus_task = (button_task_t *)btn_plus_task_to_create;
    
    for (int i = 0; i < num_ftm_responders; i++) {
        wifi_ftm_initiator_cfg_t ftm_initiator_cfg = {
            .resp_mac = {ftm_responders[i].mac[0], ftm_responders[i].mac[1], ftm_responders[i].mac[2], ftm_responders[i].mac[3], ftm_responders[i].mac[4], ftm_responders[i].mac[5]},
            .channel = ftm_responders[i].channel,
            .burst_period = 0,
            .frm_count = 0
        };

        while (esp_wifi_ftm_initiate_session(&ftm_initiator_cfg) != ESP_OK) {
            ESP_LOGE(TAG_WIFI, "Failed to initiate FTM session with responder ["MACSTR"]", MAC2STR(ftm_responders[i].mac));
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }

        EventBits_t bits = xEventGroupWaitBits(ftm_event_group,
                                                FTM_SUCCESS_BIT | FTM_FAILURE_BIT,
                                                pdTRUE,
                                                pdFALSE,
                                                portMAX_DELAY);

        if (bits & FTM_SUCCESS_BIT) {
            char post_data[255];
            sprintf(post_data, "{\"data\": {\"mac\": \"%02x:%02x:%02x:%02x:%02x:%02x\", \"rssi\": %d, \"dist_est\": %"PRIu32".%"PRIu32"}}",
                    ftm_responders[i].mac[0], ftm_responders[i].mac[1], ftm_responders[i].mac[2], ftm_responders[i].mac[3], ftm_responders[i].mac[4], ftm_responders[i].mac[5],
                    ftm_responders[i].rssi, dist_est / 100, dist_est % 100);
            ESP_LOGI(TAG_WIFI, "POST data: %s", post_data);
            ESP_LOGI(TAG_WIFI, "To url: http://"IPSTR":%d/%s", IP2STR(&server_ip), 8080, "store-data");

            esp_err_t err = http_post_data(post_data);
            if (err != ESP_OK) {
                ESP_LOGE(TAG_WIFI, "Failed to send data to server");
            }

        } else if (bits & FTM_FAILURE_BIT) {
            ESP_LOGE(TAG_WIFI, "FTM procedure with responder ["MACSTR"] failed", MAC2STR(ftm_responders[i].mac));
        } else {
            ESP_LOGE(TAG_WIFI, "Unknown event");
            ESP_ERROR_CHECK(esp_wifi_ftm_end_session());
        }
        esp_wifi_ftm_end_session();
    }

    // Re-register the button callback
    ESP_ERROR_CHECK(iot_button_register_cb(btn_plus_task->btn, BUTTON_SINGLE_CLICK, button_single_press_cb, btn_plus_task->task_to_create));

    // Free the memory allocated in the button_double_press_cb
    free(btn_plus_task);

    vTaskDelete(NULL);
}