#include "iot_servo.h"
#include "my_common.h"
#include "my_servo.h"
#include "my_pressure.h"
#include <esp_wifi.h>

bool servo_quit = false; // 舵机退出执行任务标志位
byte servo_angle;        // 记录舵机当前角度

static const char *TAG = "my_servo";

static uint16_t calibration_value_0 = 30;    // Real 0 degree angle
static uint16_t calibration_value_180 = 195; // Real 0 degree angle

TaskHandle_t beginServoTask()
{
    // 舵机控制任务句柄
    TaskHandle_t xServoTaskHandle = NULL;
    // 创建舵机控制任务
    xTaskCreate(
        servo,
        "servo",
        1024 * 3,
        NULL,
        1,
        &xServoTaskHandle);
    return xServoTaskHandle;
}

void servoSetup()
{
    ESP_LOGI(TAG, "servo setup");

    // Configure the servo
    servo_config_t servo_cfg = {
        .max_angle = 180,
        .min_width_us = 500,
        .max_width_us = 2500,
        .freq = 50,
        .timer_number = LEDC_TIMER_0,
        .channels = {
            .servo_pin = {
                SERVO_PIN,
            },
            .ch = {
                LEDC_CHANNEL_0,
            },
        },
        .channel_number = 1,
    };

    // Initialize the servo
    iot_servo_init(LEDC_LOW_SPEED_MODE, &servo_cfg);

    servo_angle = calibration_value_0;
}

// 舵机控制任务
void servo(void *pvParameters)
{
    uint32_t ulNotifiedValue;
    byte angleB, step;
    uint16_t speed;
    bool waitNotify = true; // 是否需要等待任务通知

    for (;;)
    {
#ifdef DEBUG
        Serial.printf("舵机等待指令。servo_quit=%d\n", servo_quit);
#endif

        if (waitNotify)
        {
            ulNotifiedValue = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
#ifdef DEBUG
            Serial.print("收到指令：");
            Serial.println(ulNotifiedValue);
#endif
        }
        waitNotify = true;

        switch (ulNotifiedValue)
        {
        case 0:
            // 0 正转
            angleB = calibration_value_180;
            step = 1;
            speed = 100;
            break;
        case 1:
            // 1 反转
            angleB = calibration_value_0;
            step = -1;
            speed = 50;
            break;
        default:
            angleB = calibration_value_0;
            step = 0;
            speed = 0;
            break;
        }

        // esp_wifi_set_ps(WIFI_PS_NONE);

        // while (1)
        // {
        //     // Set the angle of the servo
        //     for (int i = calibration_value_0; i <= calibration_value_180; i += 1)
        //     {
        //         iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 0, i);
        //         vTaskDelay(20 / portTICK_PERIOD_MS);
        //     }
        //     // Return to the initial position
        //     iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 0, calibration_value_0);
        //     vTaskDelay(1000 / portTICK_PERIOD_MS);
        // }

        for (; servo_angle != angleB; servo_angle += step)
        {
            byte threshold_times = 0;
#ifdef DEBUG
            Serial.print("servo angle: ");
            Serial.println(servo_angle);
#endif
            // 检查压力值
            if (ulNotifiedValue == 0 && pressure_value > PRESSURE_THRESHOLD && ++threshold_times == 3)
            {
                // 检查压力值连续3次超过阈值,舵机回缩
                break;
            }
            iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 0, servo_angle);
            vTaskDelay(pdMS_TO_TICKS(speed));
        }

        // 如果舵机正向运动中手指遇到压力或者全程没有达到压力阈值则回缩
        if (ulNotifiedValue == 0)
        {
            waitNotify = false;
            ulNotifiedValue = 1;
        } else {
            // 通知手指压力检测任务停止
            pressure_stop_signal = true;
        }
    }

    vTaskDelete(NULL);
}
