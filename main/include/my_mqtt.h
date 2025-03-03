#ifndef _MY_MQTT_H_
#define _MY_MQTT_H_


#include "PubSubClient.h"

#define MQTT_DEVICE_ID "268199f0f9b6668d1awni6"
#define MQTT_PRODUCT_ID "fzxacazsxtlfbbcq"
#define MQTT_DEVICE_SECRET "QIJ8JGSMESHS2NKx"
#define MQTT_BROKER "m1.tuyacn.com"
#define MQTT_PORT 8883

// 定义mqttCallback函数指针
typedef void (*mqttCallbackFunc)(char *, byte *, unsigned int );

extern PubSubClient mqtt_client;
extern char mqtt_property_set_topic[];
extern char mqtt_property_report_topic[];

void connectToMQTT();
void mqttSetup(mqttCallbackFunc mqttCallback);
bool mqttReportDeviceStatus(my_event_t event);

#endif