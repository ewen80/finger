#ifndef _MY_PRESSURE_H_
#define _MY_PRESSURE_H_

#include "Arduino.h"

extern TaskHandle_t xFingerMeasurePressureTaskHandle;
extern volatile uint16_t pressure_value;  // 检测压力值

TaskHandle_t beginMeasurePressureTask(void *eventGroup);
void measure(void *pvParameters);

#endif