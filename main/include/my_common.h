#ifndef _COMMON_H_
#define _COMMON_H_

#include "Arduino.h"

#define DEBUG
#define LED_PIN   2
#define PRESSURE_PIN 36
#define PRESSURE_THRESHOLD 1000 // 手指压力阈值，高于此值手指回
#define SERVO_PIN 18
#define SERVO_DEFAULT_ANGLE 0  // 舵机初始角度

// #define WIFI_SSID "HappyFamily"
// #define WIFI_PASSWORD "8310188011"
#define WIFI_SSID "guest"
#define WIFI_PASSWORD "guest@123"

#endif