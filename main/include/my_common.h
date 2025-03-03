#ifndef _COMMON_H_
#define _COMMON_H_

#include "Arduino.h"

#define DEBUG
#define LED_PIN   2
#define PRESSURE_PIN 36
#define PRESSURE_THRESHOLD 1000 // 手指压力阈值，高于此值手指回
#define SERVO_PIN GPIO_NUM_18

typedef enum {
    TOUCH_START = BIT0, // 启动手指事件
    PRESSURE_START = BIT1,  // 启动压力检测
    PRESSURE_END = BIT2,    // 压力检测结束
    SERVO_RUNBACK = BIT3,     // 舵机回缩
    SERVO_END = BIT4,     // 舵机结束
    FINGER_TOUCHED = BIT5 // 手指触碰到开关
} my_event_t;

#endif