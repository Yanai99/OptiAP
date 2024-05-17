#pragma once

#include <string.h>

#include "esp_log.h"

#include "esp_wifi.h"
#include "esp_mac.h"
#include "esp_event.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "esp_http_client.h"

#include "common.h"

typedef struct {
    uint8_t mac[6];
    uint8_t channel;
    int8_t rssi;
} ftm_responder_t;

extern EventGroupHandle_t ftm_event_group;
#define FTM_SUCCESS_BIT BIT0
#define FTM_FAILURE_BIT BIT1

extern uint32_t rtt_raw;
extern uint32_t rtt_est;
extern uint32_t dist_est;

#define MAX_FTM_RESPONDERS 64

extern esp_ip4_addr_t server_ip;

#define TAG_WIFI "WIFI"

/**
 * @brief Event handler for Wi-Fi events
 * 
 * @param arg Pointer to the event data
 * @param event_base Event base
 * @param event_id Event ID
 * @param event_data Pointer to the event data
 * 
 * @return void
 */
void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data);

/**
 * @brief Event handler for IP events
 * 
 * @param arg Pointer to the event data
 * @param event_base Event base
 * @param event_id Event ID
 * @param event_data Pointer to the event data
 * 
 * @return void
 */
void ip_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

/**
 * @brief Event handler for HTTP events
 * 
 * @param event Pointer to the event data
 * 
 * @return ESP_OK if the event was successfully handled, ESP_FAIL otherwise
 */
esp_err_t http_event_handler(esp_http_client_event_t *event);

/**
 * @brief Initialize the Wi-Fi as a station and as an access point
 * 
 * @param netif_sta Pointer to the network interface for the station
 * @param netif_ap Pointer to the network interface for the access point
 * 
 * @return ESP_OK if the Wi-Fi was successfully initialized, ESP_FAIL otherwise
 */
esp_err_t wifi_init(esp_netif_t* netif_sta, esp_netif_t* netif_ap);

/**
 * @brief Scan for FTM responders
 *        
 * 
 * @param num_ftm_responders Pointer to the number of FTM responders
 * @param ftm_responders Pointer to the array of FTM responders
 * 
 * @return void
 */
void scan_ftm_responders(int* num_ftm_responders, ftm_responder_t* ftm_responders);

/**
 * @brief Perform the FTM procedure with the FTM responders stored in the global variable ftm_responders
 * 
 * @param nothing Unused parameter
 * 
 * @return void
 */
void ftm_procedure(void *btn_plus_task_to_create);

esp_err_t http_post_data(esp_http_client_handle_t client, char *post_data);