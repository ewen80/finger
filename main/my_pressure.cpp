#include "my_common.h"
#include "my_pressure.h"
#include "my_servo.h"
#include "main.h"

volatile uint16_t pressure_value; 

static const char *TAG = "my_pressure";

TaskHandle_t beginMeasurePressureTask(void *eventGroup)
{
    // 手指压力检测任务句柄
    TaskHandle_t xFingerMeasurePressureTaskHandle = NULL;
    // 创建手指压力检测任务
    xTaskCreate(
        measure,
        "measure",
        1024 * 2,
        eventGroup,
        1,
        &xFingerMeasurePressureTaskHandle);
    return xFingerMeasurePressureTaskHandle;
}


// 手指压力检测任务
void measure(void *pvParameters)
{
    for (;;)
    {
        xEventGroupWaitBits(xEventGroup,
        my_event_t::TOUCH_START,
        pdTRUE,   // 等待后清除事件位
        pdFALSE,  // 不要求所有位都置位
        portMAX_DELAY);
        ESP_LOGI(TAG, "接收到TOUCH_START事件");

        // 通知舵机任务启动
        xEventGroupSetBits(xEventGroup, my_event_t::PRESSURE_START);
        ESP_LOGI(TAG, "发送PRESSURE_START事件");
        while (!(xEventGroupGetBits(xEventGroup) & my_event_t::SERVO_END))
        {
            // 检测压力值   
            pressure_value = analogRead(PRESSURE_PIN);
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        // 收到舵机停止事件后发送检测结束事件
        // 手动清除事件位
        xEventGroupClearBits(xEventGroup, my_event_t::SERVO_END);
        ESP_LOGI(TAG, "接收到SERVO_END事件");
        xEventGroupSetBits(xEventGroup, my_event_t::PRESSURE_END);
        ESP_LOGI(TAG, "发送PRESSURE_END事件");

    }
    vTaskDelete(NULL);
}