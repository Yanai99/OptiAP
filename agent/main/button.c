#include "button.h"

#include "freertos/FreeRTOS.h"

esp_err_t init_button(button_handle_t *btn)
{
    /* Button configuration */
    button_config_t button_config = {
        .type = BUTTON_TYPE_GPIO,
        .gpio_button_config = {
            .gpio_num = BUTTON_GPIO,
            .active_level = 0
        }
    };

    /* Create a new button with the configuration */
    *btn = iot_button_create(&button_config);
    if (*btn == NULL)
    {
        ESP_LOGE(TAG_BUTTON, "Failed to create button");
        return ESP_FAIL;
    }

    return ESP_OK;
}

void button_single_press_cb(void *btn, void *task_to_create)
{
    ESP_LOGI(TAG_BUTTON, "Button single press");
    iot_button_unregister_cb((button_handle_t)btn, BUTTON_SINGLE_CLICK);
    TaskFunction_t task = (TaskFunction_t)task_to_create;
    button_task_t *btn_plus_task = (button_task_t *)malloc(sizeof(button_task_t)); // we need to free this memory in the task
    btn_plus_task->btn = (button_handle_t)btn;
    btn_plus_task->task_to_create = task_to_create;
    xTaskCreate(task, "single_press_task", 4096, btn_plus_task, 1, NULL);
}

void button_double_press_cb(void *btn, void *task_to_create)
{
    ESP_LOGI(TAG_BUTTON, "Button double press");
    iot_button_unregister_cb((button_handle_t)btn, BUTTON_DOUBLE_CLICK);
    TaskFunction_t task = (TaskFunction_t)task_to_create;
    button_task_t *btn_plus_task = (button_task_t *)malloc(sizeof(button_task_t)); // we need to free this memory in the task
    btn_plus_task->btn = (button_handle_t)btn;
    btn_plus_task->task_to_create = task_to_create;
    xTaskCreate(task, "double_press_task", 4096, btn_plus_task, 1, NULL);
}

void button_long_press_cb(void *btn, void *task_to_create)
{
    ESP_LOGI(TAG_BUTTON, "Button long press");
    iot_button_unregister_cb((button_handle_t)btn, BUTTON_LONG_PRESS_START);
    TaskFunction_t task = (TaskFunction_t)task_to_create;
    button_task_t *btn_plus_task = (button_task_t *)malloc(sizeof(button_task_t)); // we need to free this memory in the task
    btn_plus_task->btn = (button_handle_t)btn;
    btn_plus_task->task_to_create = task_to_create;
    xTaskCreate(task, "long_press_task", 4096, btn_plus_task, 1, NULL);
}

esp_err_t assign_functionalities_to_button(button_handle_t *btn, TaskFunction_t task_to_create1, TaskFunction_t task_to_create2, TaskFunction_t task_to_create3)
{
    if (task_to_create1 == NULL) {
        ESP_LOGW(TAG_BUTTON, "Task to create for single press is NULL");
    } else {
        ESP_ERROR_CHECK(iot_button_register_cb(*btn, BUTTON_SINGLE_CLICK, button_single_press_cb, task_to_create1));
    }

    if (task_to_create2 == NULL) {
        ESP_LOGW(TAG_BUTTON, "Task to create for double press is NULL");
    } else {
        ESP_ERROR_CHECK(iot_button_register_cb(*btn, BUTTON_DOUBLE_CLICK, button_double_press_cb, task_to_create2));
    }

    if (task_to_create3 == NULL) {
        ESP_LOGW(TAG_BUTTON, "Task to create for long press is NULL");
    } else {
        ESP_ERROR_CHECK(iot_button_register_cb(*btn, BUTTON_LONG_PRESS_START, button_long_press_cb, task_to_create3));
    }
    
    return ESP_OK;
}