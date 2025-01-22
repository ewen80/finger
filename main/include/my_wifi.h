#ifndef _MY_WIFI_H_
#define _MY_WIFI_H_

#include "WiFiClientSecure.h"


extern WiFiClientSecure esp_client;

void connectToWiFi();

#endif