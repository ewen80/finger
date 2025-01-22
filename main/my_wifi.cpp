#include "common.h"
#include "WiFiClientSecure.h"
#include "WiFi.h"

// 定义 Wi-Fi 名与密码
//const char * wifi_ssid = "guest";
//const char * wifi_password = "guest@123";
const char * wifi_ssid = "HappyFamily";
const char * wifi_password = "8310188011";

WiFiClientSecure esp_client;

void connectToWiFi() {
  WiFi.disconnect(true);
  WiFi.begin(wifi_ssid, wifi_password);
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