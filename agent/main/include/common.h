#pragma once

#include "button.h"

typedef struct
{
    button_handle_t btn;
    TaskFunction_t task_to_create;
} button_task_t;