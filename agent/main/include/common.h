#pragma once

#include "freertos/event_groups.h"

#include "button.h"

typedef struct
{
    button_handle_t btn;
    TaskFunction_t task_to_create;
} button_task_t;

extern EventGroupHandle_t server_connect_event_group;
#define SERVER_CONNECT_BIT BIT0