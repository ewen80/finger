#include "my_common.h"
#include <NTPClient.h>
#include <WiFiUdp.h>


WiFiUDP udp;
NTPClient time_client(udp, 3600*8);

void ntpSetup(){
  time_client.begin();
}

unsigned long getTimeStamp(){
  time_client.update();
  return time_client.getEpochTime();
}