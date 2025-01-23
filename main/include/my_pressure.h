#ifndef _MY_PRESSURE_H_
#define _MY_PRESSURE_H_

#include "Arduino.h"

extern TaskHandle_t xFingerMeasurePressureTaskHandle;

TaskHandle_t beginMeasurePressureTask();
void measure(void *pvParameters);

#endif