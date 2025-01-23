#include "my_common.h"
#include "my_led.h"

volatile byte led_status = 0;

void beginLedTask(){
  // 创建LED状态控制任务
  xTaskCreate(
      led,                      // 任务名称
      "LED",                    // 任务名称字符串
      1024*2, // 任务堆栈大小
      NULL,                     // 传递的参数
      1,                        // 任务优先级
      NULL);                    // 任务句柄
}


// LED灯任务 status: 0 暗 1 亮 2 闪
void led(void *pvParameters) {
  pinMode(LED_PIN, OUTPUT);
  
  while(1){
    switch(led_status){
        case 0:
          digitalWrite(LED_PIN, LOW);
          break;
        case 1:
          digitalWrite(LED_PIN, HIGH);
          break;
        case 2:
          digitalWrite(LED_PIN, !digitalRead(LED_PIN));
          break;
      }
      vTaskDelay(pdMS_TO_TICKS(500));
  }
  vTaskDelete(NULL);
}