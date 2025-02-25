#include "iot_servo.h"
#include "my_common.h"
#include "my_servo.h"
#include "my_pressure.h"
#include "main.h"

byte servo_angle;        // 记录舵机当前角度

static const char *TAG = "my_servo";

static uint16_t calibration_value_0 = 30;    // Real 0 degree angle
static uint16_t calibration_value_180 = 195; // Real 0 degree angle

TaskHandle_t beginServoTask(void *eventGroup)
{
    // 舵机控制任务句柄
    TaskHandle_t xServoTaskHandle = NULL;
    // 创建舵机控制任务
    xTaskCreate(
        servo,
        "servo",
        1024 * 3,
        eventGroup,
        1,
        &xServoTaskHandle);
    return xServoTaskHandle;
}

void servoSetup()
{
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

void servoDetach()
{
    iot_servo_deinit(LEDC_LOW_SPEED_MODE);
}

// 舵机控制任务
void servo(void *pvParameters)
{
    bool runback = false;   // 是否需要回缩
    byte angleB, step;
    uint16_t speed;

    for (;;)
    {
        EventBits_t recEvents = xEventGroupWaitBits(xEventGroup,
            my_event_t::PRESSURE_START | my_event_t::SERVO_RUNBACK,
            pdTRUE,   // 等待后清除事件位
            pdFALSE,  // 不要求所有位都置位
            portMAX_DELAY);
        if(recEvents & my_event_t::PRESSURE_START)
        {
            runback = false;
            ESP_LOGI(TAG, "收到PRESSURE_START事件");
        }
        if(recEvents & my_event_t::SERVO_RUNBACK)
        {
            runback = true;
            ESP_LOGI(TAG, "收到SERVO_RUNBACK事件");
        }
        

        if(runback)
        {
            angleB = calibration_value_0;
            step = -1;
            speed = 50;
        } else {
            angleB = calibration_value_180;
            step = 1;
            speed = 100;
        }

        for (; servo_angle != angleB; servo_angle += step)
        {
            byte threshold_times = 0;
            // 检查压力值
            if (!runback && pressure_value > PRESSURE_THRESHOLD && ++threshold_times == 1)
            {
                // 检查压力值连续3次超过阈值,舵机回缩
                fingerTouched = true;
                break;
            }
            iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 0, servo_angle);
            vTaskDelay(pdMS_TO_TICKS(speed));
        }

        // 如果舵机正向运动中手指遇到压力或者全程没有达到压力阈值则回缩
        if (!runback)
        {
            runback = true;
            xEventGroupSetBits(xEventGroup, my_event_t::SERVO_RUNBACK);
            ESP_LOGI(TAG, "发送SERVO_RUNBACK事件");
        }
        else
        {
            // 通知手指压力检测任务停止
            xEventGroupSetBits(xEventGroup, my_event_t::SERVO_END);
            vTaskDelay(pdMS_TO_TICKS(100));
            ESP_LOGI(TAG, "发送SERVO_END事件");
        }
    }

    vTaskDelete(NULL);
}
