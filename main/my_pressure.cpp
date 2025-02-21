#include "my_common.h"
#include "my_pressure.h"
#include "my_servo.h"

TaskHandle_t beginMeasurePressureTask()
{
    // 手指压力检测任务句柄
    TaskHandle_t xFingerMeasurePressureTaskHandle = NULL;
    // 创建手指压力检测任务
    xTaskCreate(
        measure,
        "measure",
        1024 * 2,
        NULL,
        1,
        &xFingerMeasurePressureTaskHandle);
    return xFingerMeasurePressureTaskHandle;
}


// 手指压力检测任务
void measure(void *pvParameters)
{
    uint32_t ulNotifiedValue;
    for (;;)
    {
        // 尝试从通知中取值，如果没有则挂起
        ulNotifiedValue = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (ulNotifiedValue > 0)
        {
            // 通知舵机任务启动
            xTaskNotify(xServoTaskHandle, 0, eSetValueWithOverwrite);

            pressure_stop_signal = false;

            while (!pressure_stop_signal)
            {
                // 检测压力值   
                pressure_value = analogRead(PRESSURE_PIN);
                vTaskDelay(pdMS_TO_TICKS(100));
            }
        }
    }
    vTaskDelete(NULL);
}