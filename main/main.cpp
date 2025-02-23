#include "my_common.h"
#include <ArduinoJson.h>
#include "main.h"
#include "my_wifi_idf.h"
#include "my_ntp.h"
#include "my_mqtt.h"
#include "my_led.h"
#include "my_pressure.h"
#include "my_servo.h"

#include <esp_wifi.h>
#include "driver/ledc.h"

static const char *TAG = "my_main";

// led任务句柄
TaskHandle_t xLEDTaskHandle = NULL;
// 手指压力检测任务句柄
TaskHandle_t xFingerMeasurePressureTaskHandle = NULL;
// 舵机控制任务句柄
TaskHandle_t xServoTaskHandle = NULL;
// 事件组句柄
EventGroupHandle_t xEventGroup;

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
void fingerTouch()
{
    // 关闭系统节能
    esp_wifi_set_ps(WIFI_PS_NONE);    
    xEventGroupSetBits(xEventGroup, my_event_t::TOUCH_START);
    ESP_LOGI(TAG, "发送手指触发事件");
    xEventGroupWaitBits(xEventGroup, my_event_t::PRESSURE_END, pdTRUE, pdTRUE, portMAX_DELAY);
    ESP_LOGI(TAG, "收到PRESSURE_END事件");

    ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
    // 打开系统节能
    esp_wifi_set_ps(DEFAULT_PS_MODE);
}

void setup()
{
  // 创建事件组
  xEventGroup = xEventGroupCreate();

  if (xEventGroup != NULL)
  {
    // 创建LED任务
    xLEDTaskHandle = beginLedTask(xEventGroup);
    // 创建手指压力检测任务
    xFingerMeasurePressureTaskHandle = beginMeasurePressureTask(xEventGroup);
    // 创建舵机控制任务
    xServoTaskHandle = beginServoTask(xEventGroup);
  }

  setLedFlash(xLEDTaskHandle);

#ifdef DEBUG
  Serial.begin(115200);
  while (!Serial)
  {
    ; // wait for serial port to connect
  }
#endif
  analogReadResolution(12);

  // 连接WIFI
  wifiSetup();
  // 初始化NTP
  ntpSetup();
  // 连接mqtt
  mqttSetup(mqttCallback);
  servoSetup();

}

void loop()
{
  if (!mqtt_client.connected())
  {
    setLedFlash(xLEDTaskHandle);
    connectToMQTT();
  }
  else
  {
    setLedOff(xLEDTaskHandle);
  }
  mqtt_client.loop();

  vTaskDelay(pdMS_TO_TICKS(5000));
}