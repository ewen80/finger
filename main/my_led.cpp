#include "my_common.h"
#include "my_led.h"

volatile byte led_status = 0;

TaskHandle_t beginLedTask()
{

  TaskHandle_t task;
  // 创建LED状态控制任务
  xTaskCreate(
      led,      // 任务名称
      "LED",    // 任务名称字符串
      1024 * 2, // 任务堆栈大小
      NULL,     // 传递的参数
      1,        // 任务优先级
      &task);   // 任务句柄
  return task;
}

void setLedON(TaskHandle_t led_task){
  led_status = 1;
  xTaskNotifyGive(led_task);
}

void setLedOff(TaskHandle_t led_task){
  led_status = 0;
  xTaskNotifyGive(led_task);
}

void setLedFlash(TaskHandle_t led_task){
  led_status = 2;
  xTaskNotifyGive(led_task);
}

// LED灯任务 status: 0 暗 1 亮 2 闪
void led(void *pvParameters)
{
  pinMode(LED_PIN, OUTPUT);

  uint32_t ulNotifiedValue;

  while (1)
  {
    ulNotifiedValue = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    if (ulNotifiedValue > 0)
    {
      switch (led_status)
      {
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
  }
  vTaskDelete(NULL);
}