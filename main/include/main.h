#ifndef _MAIN_H_
#define _MAIN_H_

typedef enum {
    TOUCH_START = BIT0, // 启动手指事件
    PRESSURE_START = BIT1,  // 启动压力检测
    PRESSURE_END = BIT2,    // 压力检测结束
    SERVO_RUNBACK = BIT3,     // 舵机回缩
    SERVO_END = BIT4     // 舵机结束
} my_event_t;

extern EventGroupHandle_t xEventGroup;


void fingerTouch();

#endif