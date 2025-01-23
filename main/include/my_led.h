#ifndef _MY_LED_H_
#define _MY_LED_H_

#include "Arduino.h"

extern volatile byte led_status; // 保存led状态 0：暗 1：亮 2：闪

void beginLedTask();
void led(void *pvParameters);

#endif