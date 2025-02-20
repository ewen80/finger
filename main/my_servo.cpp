#include "iot_servo.h"
#include "my_common.h"
#include "my_servo.h"

volatile bool servo_quit = false; // 舵机退出执行任务标志位
volatile byte servo_angle;        // 记录舵机当前角度

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
    ESP_LOGI(TAG, "舵机初始化");

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
    // for (;;)
    {
#ifdef DEBUG
        Serial.printf("舵机等待指令。servo_quit=%d\n", servo_quit);
#endif

        // if (waitNotify)
        {
            ulNotifiedValue = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
#ifdef DEBUG
            Serial.print("收到指令：");
            Serial.println(ulNotifiedValue);
#endif
        }
        // waitNotify = true;

        // switch (ulNotifiedValue)
        // {
        // case 0:
        //     // 0 正转
        //     angleB = calibration_value_180;
        //     step = 1;
        //     speed = 100;
        //     break;
        // case 1:
        //     // 1 反转
        //     angleB = calibration_value_0;
        //     step = -1;
        //     speed = 50;
        //     break;
        // default:
        //     angleB = calibration_value_0;
        //     step = 0;
        //     speed = 0;
        //     break;
        // }

        while (1)
        {
            // Set the angle of the servo
            for (int i = calibration_value_0; i <= calibration_value_180; i += 1)
            {
                iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 0, i);
                vTaskDelay(20 / portTICK_PERIOD_MS);
            }
            // Return to the initial position
            iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 0, calibration_value_0);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }

        //         for (; servo_angle != angleB; servo_angle += step)
        //         {
        //             // 是否收到舵机停止指令
        //             if (!servo_quit)
        //             {
        // #ifdef DEBUG
        //                 Serial.print("舵机：");
        //                 Serial.println(servo_angle);
        // #endif
        //                 iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 0, servo_angle);
        //                 vTaskDelay(pdMS_TO_TICKS(speed));
        //             }
        //             else
        //             {
        //                 // 收到舵机停止指令，继续等待任务通知
        // #ifdef DEBUG
        //                 Serial.println("舵机停止");
        // #endif
        //                 break;
        //             }
        //         }

        // // 如果手指全程没有达到压力阈值则回缩
        // if (!servo_quit && ulNotifiedValue == 0)
        // {
        //     waitNotify = false;
        //     ulNotifiedValue = 1;
        // }
        // servo_quit = false;
    }

    vTaskDelete(NULL);
}
