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

// 检测压力值并在到达阈值后返回true
bool measurePressure()
{
    int threshold_times = 0;
    unsigned int pressure_value;
    while (1)
    {
        pressure_value = analogRead(PRESSURE_PIN);
        if (pressure_value > PRESSURE_THRESHOLD)
        {
            if (++threshold_times == 3)
            {
                // 如果连续3次超过阈值
                return true;
            }
        }

#ifdef DEBUG
        //      Serial.printf("ADC analog value = %d\n", pressure_value);
#endif
        vTaskDelay(pdMS_TO_TICKS(500));
    }
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
            if (measurePressure())
            {
                // 压力达到阈值,通知舵机复位
                servo_quit = true;
                vTaskDelay(pdMS_TO_TICKS(100));
                xTaskNotify(xServoTaskHandle, 1, eSetValueWithOverwrite);
            }
        }
    }
    vTaskDelete(NULL);
}