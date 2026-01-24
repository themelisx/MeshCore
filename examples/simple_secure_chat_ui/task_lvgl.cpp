#include <Arduino.h>

#include "esp_log.h"
#include "uiDefines.h"
#include "uiVars.h"

void lvgl_task(void *pvParameters) {

  vTaskSuspend(NULL);

  ESP_LOGI("UI manager: Task running on core %d", xPortGetCoreID());

  while (1) {    
    lv_timer_handler();
    vTaskDelay(DELAY_LVGL_TASK / portTICK_PERIOD_MS);
  }
}