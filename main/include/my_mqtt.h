#ifndef _MY_MQTT_H_
#define _MY_MQTT_H_


#include "PubSubClient.h"

// 定义mqttCallback函数指针
typedef void (*mqttCallbackFunc)(char *, byte *, unsigned int );

extern PubSubClient mqtt_client;

void connectToMQTT();
void mqttSetup(mqttCallbackFunc mqttCallback);
bool mqttPublish(const char *topic, const char *payload);

#endif