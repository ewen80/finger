#include "Arduino.h"
#include "my_common.h"
#include <ESP32Servo.h>
#include "my_servo.h"

volatile bool servo_quit = false; // 舵机退出执行任务标志位
volatile byte servo_angle;        // 记录舵机当前角度

Servo myservo;

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
    // Allow allocation of all timers
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);
    myservo.setPeriodHertz(50);           // standard 50 hz servo
    myservo.attach(SERVO_PIN, 500, 2500); // attaches the servo on pin 18 to the servo object
    // using default min/max of 500us and 2500us
    // different servos may require different min/max settings
    // for an accurate 0 to 180 sweep

    // 舵机初始角度
    servo_angle = SERVO_DEFAULT_ANGLE;
    myservo.write(servo_angle);
    vTaskDelay(pdMS_TO_TICKS(500));
    // myservo.detach();
    // vTaskDelay(pdMS_TO_TICKS(500));
}

// 舵机控制任务
void servo(void *pvParameters)
{
    uint32_t ulNotifiedValue;
    byte angleB, step, speed;
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
            angleB = 180;
            step = 1;
            speed = 500;
            break;
        case 1:
            // 1 反转
            angleB = 0;
            step = -1;
            speed = 150;
            break;
        default : 
            angleB = 0;
            step = 0;
            speed = 0;
            break;
        }

#ifdef DEBUG
                Serial.printf("绑定舵机");
#endif

        // // 舵机准备启动，绑定舵机
        // myservo.attach(SERVO_PIN, 500, 2500);
        // vTaskDelay(pdMS_TO_TICKS(500));

        for (; servo_angle != angleB; servo_angle += step)
        {
            // 是否收到舵机停止指令
            if (!servo_quit)
            {
#ifdef DEBUG
                Serial.print("舵机：");
                Serial.println(servo_angle);
#endif
                myservo.write(servo_angle);
                vTaskDelay(pdMS_TO_TICKS(speed));
            }
            else
            {
                // 收到舵机停止指令，继续等待任务通知
                // myservo.detach();
#ifdef DEBUG
                Serial.println("舵机停止");
#endif
                break;
            }
        }

#ifdef DEBUG
                Serial.printf("解绑舵机");
#endif
        // // 停止转动，解绑舵机
        // myservo.detach();
        // vTaskDelay(pdMS_TO_TICKS(500));

        // 如果手指全程没有达到压力阈值则回缩
        if (!servo_quit && ulNotifiedValue == 0)
        {
            waitNotify = false;
            ulNotifiedValue = 1;
        }
        servo_quit = false;
    }
    vTaskDelete(NULL);
}
