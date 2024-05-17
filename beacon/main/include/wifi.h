#pragma once

#include <string.h>

#include "esp_log.h"
#include "esp_wifi.h"

#define WIFI_SSID "Beacon"
#define WIFI_PASS "password"

#define TAG_WIFI "WIFI"

esp_netif_t* wifi_init(void);