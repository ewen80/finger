#ifndef _MY_LED_H_
#define _MY_LED_H_

#include "Arduino.h"

extern TaskHandle_t xLEDTaskHandle;

TaskHandle_t beginLedTask();
void led(void *pvParameters);
void setLedON(TaskHandle_t led_task);
void setLedOff(TaskHandle_t led_task);
void setLedFlash(TaskHandle_t led_task);

#endif