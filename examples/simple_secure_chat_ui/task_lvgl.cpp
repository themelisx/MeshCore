#include <Arduino.h>

#include "defines.h"
//#include <MyDebug.h>
#include "vars.h"

void lvgl_task(void *pvParameters) {

  vTaskSuspend(NULL);

  Serial.printf("UI manager: Task running on core %d", xPortGetCoreID());
  Serial.println();

  while (1) {    
    lv_timer_handler();
    vTaskDelay(DELAY_LVGL_TASK / portTICK_PERIOD_MS);
  }
  // myDebug->println(DEBUG_LEVEL_INFO, "Terminating UI manager");
  // vTaskDelete(NULL);
}