#ifndef _MY_MQTT_H_
#define _MY_MQTT_H_


#include "PubSubClient.h"

extern PubSubClient mqtt_client;

void connectToMQTT();

#endif