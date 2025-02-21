#ifndef _MY_PRESSURE_H_
#define _MY_PRESSURE_H_

#include "Arduino.h"

extern TaskHandle_t xFingerMeasurePressureTaskHandle;
volatile uint16_t pressure_value;  // 检测压力值
volatile bool pressure_stop_signal; // 停止检测信号

TaskHandle_t beginMeasurePressureTask();
void measure(void *pvParameters);

#endif