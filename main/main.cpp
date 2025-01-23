#include "my_common.h"
#include <ArduinoJson.h>
#include "main.h"
#include "my_wifi.h"
#include "my_ntp.h"
#include "my_mqtt.h"
#include "my_led.h"
#include "my_pressure.h"
#include "my_servo.h"

static const char *TAG = "My_Debug";

// 手指压力检测任务句柄
TaskHandle_t xFingerMeasurePressureTaskHandle = NULL;
// 舵机控制任务句柄
TaskHandle_t xServoTaskHandle = NULL;

// mqtt回调函数
void mqttCallback(char *mqtt_topic, byte *payload, unsigned int length)
{
#ifdef DEBUG
  Serial.print("Message received on mqtt_topic: ");
  Serial.println(mqtt_topic);
  Serial.print("Message: ");

  for (unsigned int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println("\n-----------------------");
#endif

  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, payload);
  if (error)
  {
#ifdef DEBUG
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
#endif
    return;
  }

  String topic = String(mqtt_topic).substring(29);
  // 触发手指开关指令
  if (topic == "/thing/action/execute")
  {
    fingerTouch();
  }
}

// 手指开始触发
void fingerTouch(){
  // 启动压力检测
  xTaskNotifyGive(xFingerMeasurePressureTaskHandle);
  // 通知舵机正传
  xTaskNotify(xServoTaskHandle, 0, eSetValueWithOverwrite);

}

void setup()
{
  led_status = 2;
#ifdef DEBUG
  Serial.begin(115200);
  while (!Serial)
  {
    ; // wait for serial port to connect
  }
#endif
  analogReadResolution(12);

  // 创建LED任务
  beginLedTask();
  // 创建手指压力检测任务
  xFingerMeasurePressureTaskHandle =  beginMeasurePressureTask();
  // 创建舵机控制任务
  xServoTaskHandle = beginServoTask();

  // 连接WIFI
  connectToWiFi();
  // 初始化NTP
  ntpSetup();
  // 初始化舵机
  servoSetup();
  // 连接mqtt
  mqttSetup(mqttCallback);

  led_status = 1;
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (!mqtt_client.connected())
  {
    connectToMQTT();
  }
  mqtt_client.loop();
  delay(500);
}