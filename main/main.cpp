#include "Arduino.h"
#include "common.h"
#include "my_wifi.h"
#include "my_ntp.h"
#include "my_mqtt.h"


void setup()
{
#ifdef DEBUG
  Serial.begin(115200);
  while (!Serial)
  {
    ; // wait for serial port to connect
  }
#endif
  analogReadResolution(12);

  // 连接WIFI
  connectToWiFi();
  // 初始化NTP
  initNTP();
}

void loop()
{
    // put your main code here, to run repeatedly:
   if (!mqtt_client.connected()) {
    connectToMQTT();
   }
   mqtt_client.loop();
}