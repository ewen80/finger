#include "my_common.h"
#include <NTPClient.h>
#include <WiFiUdp.h>


WiFiUDP udp;
const char* ntpServer = "ntp.aliyun.com";
NTPClient time_client(udp, ntpServer, 0);


void ntpSetup(){
  time_client.begin();
}

unsigned long getTimeStamp(){
  time_client.update();
  return time_client.getEpochTime();
}