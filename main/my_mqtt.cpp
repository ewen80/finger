#include "my_common.h"
#include "Crypto.h"
#include "SHA256.h"

#include "my_wifi.h"
#include "my_mqtt.h"
#include "my_ntp.h"

// MQTT Broker settings
const char *mqtt_broker = "m1.tuyacn.com";
const int mqtt_port = 8883;
const char *mqtt_device_id = "268199f0f9b6668d1awni6";
const char *mqtt_product_id = "fzxacazsxtlfbbcq";
const char *mqtt_device_secret = "QIJ8JGSMESHS2NKx";
char mqtt_username[] = "6c828cba434ff40c074wF2|signMethod=hmacSha256,timestamp=1607837283,secureMode=1,accessType=1";
char mqtt_password[] = "9088f1608df4744e2a933ff905ffdde58dc7213510f25ad786a89896a5ea1104";

// mqtt服务器证书
const char *ca_cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDxTCCAq2gAwIBAgIBADANBgkqhkiG9w0BAQsFADCBgzELMAkGA1UEBhMCVVMx
EDAOBgNVBAgTB0FyaXpvbmExEzARBgNVBAcTClNjb3R0c2RhbGUxGjAYBgNVBAoT
EUdvRGFkZHkuY29tLCBJbmMuMTEwLwYDVQQDEyhHbyBEYWRkeSBSb290IENlcnRp
ZmljYXRlIEF1dGhvcml0eSAtIEcyMB4XDTA5MDkwMTAwMDAwMFoXDTM3MTIzMTIz
NTk1OVowgYMxCzAJBgNVBAYTAlVTMRAwDgYDVQQIEwdBcml6b25hMRMwEQYDVQQH
EwpTY290dHNkYWxlMRowGAYDVQQKExFHb0RhZGR5LmNvbSwgSW5jLjExMC8GA1UE
AxMoR28gRGFkZHkgUm9vdCBDZXJ0aWZpY2F0ZSBBdXRob3JpdHkgLSBHMjCCASIw
DQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAL9xYgjx+lk09xvJGKP3gElY6SKD
E6bFIEMBO4Tx5oVJnyfq9oQbTqC023CYxzIBsQU+B07u9PpPL1kwIuerGVZr4oAH
/PMWdYA5UXvl+TW2dE6pjYIT5LY/qQOD+qK+ihVqf94Lw7YZFAXK6sOoBJQ7Rnwy
DfMAZiLIjWltNowRGLfTshxgtDj6AozO091GB94KPutdfMh8+7ArU6SSYmlRJQVh
GkSBjCypQ5Yj36w6gZoOKcUcqeldHraenjAKOc7xiID7S13MMuyFYkMlNAJWJwGR
tDtwKj9useiciAF9n9T521NtYJ2/LOdYq7hfRvzOxBsDPAnrSTFcaUaz4EcCAwEA
AaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMCAQYwHQYDVR0OBBYE
FDqahQcQZyi27/a9BUFuIMGU2g/eMA0GCSqGSIb3DQEBCwUAA4IBAQCZ21151fmX
WWcDYfF+OwYxdS2hII5PZYe096acvNjpL9DbWu7PdIxztDhC2gV7+AJ1uP2lsdeu
9tfeE8tTEH6KRtGX+rcuKxGrkLAngPnon1rpN5+r5N9ss4UXnT3ZJE95kTXWXwTr
gIOrmgIttRD02JDHBHNA7XIloKmf7J6raBKZV8aPEjoJpL1E/QYVN8Gb5DKj7Tjo
2GTzLH4U/ALqn83/B2gX2yKQOC16jdFU8WnjXzPKej17CuPKf1855eJ1usV2GDPO
LPAvTK33sefOT6jEm0pUBsV/fdUID+Ic/n4XuKxe9tQWskMJDE32p2u0mYRlynqI
4uJEvlz36hz1
-----END CERTIFICATE-----
)EOF";

PubSubClient mqtt_client(esp_client);
SHA256 sha256;

void setMqttUsername(){
  sprintf(mqtt_username, "%s|signMethod=hmacSha256,timestamp=%ld,secureMode=1,accessType=1", mqtt_device_id, getTimeStamp());
  #ifdef DEBUG
    Serial.print("username:");
    Serial.println(mqtt_username);
  #endif
}

// 将字节数组转为对应的16进制字符串，使用完记得释放空间
void bytes_to_hex_string_snprintf(const unsigned char *bytes, size_t length, char *dest) {
    for (size_t i = 0; i < length; ++i) {
        snprintf(&dest[i * 2], 3, "%02x", bytes[i]); // 每次最多写入两位16进制数加一个终止符
    }
}

void setMqttPassword(){
  char content[] = "deviceId=6c828cba434ff40c074wF2,timestamp=1607635284,secureMode=1,accessType=1";
  uint8_t len = sha256.hashSize();
  uint8_t result[len];
  
  sprintf(content, "deviceId=%s,timestamp=%ld,secureMode=1,accessType=1", mqtt_device_id, getTimeStamp());
  sha256.resetHMAC(mqtt_device_secret, strlen(mqtt_device_secret));
  sha256.update(content, strlen(content));
  sha256.finalizeHMAC(mqtt_device_secret, strlen(mqtt_device_secret), result, len);

  bytes_to_hex_string_snprintf(result, len, mqtt_password);

  #ifdef DEBUG
    Serial.print("password:");
    Serial.println(mqtt_password);
  #endif
}

void connectToMQTT() {
  while (!mqtt_client.connected()) {
    String client_id = "tuyalink_";
    client_id += mqtt_device_id;
    #ifdef DEBUG
      Serial.printf("Connecting to MQTT Broker as %s.....\n", client_id.c_str());
    #endif
    setMqttUsername();
    setMqttPassword();
    if (mqtt_client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      #ifdef DEBUG
        Serial.println("Connected to MQTT broker");
      #endif
      //订阅动作
      char execute_topic[] = "tylink/268199f0f9b6668d1awni6/thing/action/execute";
      sprintf(execute_topic, "tylink/%s/thing/action/execute", mqtt_device_id);
      mqtt_client.subscribe(execute_topic);
    } else {
      #ifdef DEBUG
        Serial.print("Failed, rc=");
        Serial.print(mqtt_client.state());
        Serial.println(" try again in 5 seconds");
      #endif
        delay(5000);
    }
   }
}





// mqtt配置
void mqttSetup(mqttCallbackFunc mqttCallback){
  esp_client.setCACert(ca_cert);
  
  mqtt_client.setServer(mqtt_broker, mqtt_port);
  mqtt_client.setKeepAlive(60);
  mqtt_client.setCallback(mqttCallback); // Corrected callback function name

}

