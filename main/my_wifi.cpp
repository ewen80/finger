#include "my_common.h"
#include "WiFiClientSecure.h"
#include "WiFi.h"

WiFiClientSecure esp_client;

void connectToWiFi() {
  WiFi.disconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  #ifdef DEBUG
    Serial.print("Connecting to WiFi");
  #endif
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      #ifdef DEBUG
        Serial.print(".");
      #endif
  }
  #ifdef DEBUG
    Serial.println("\nConnected to WiFi");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());   
  #endif
}