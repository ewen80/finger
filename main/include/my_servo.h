#ifndef _MY_SERVO_H_
#define _MY_SERVO_H_

#include "Arduino.h"

// extern volatile bool servo_quit; // 舵机退出执行任务标志位
// extern volatile byte servo_angle; // 记录舵机当前角度
extern TaskHandle_t xServoTaskHandle;

TaskHandle_t beginServoTask(void *eventGroup);
void servoSetup();
void servo(void *pvParameters);

#endif