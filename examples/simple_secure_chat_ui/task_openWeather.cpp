#include <Arduino.h>

#include "esp_log.h"
#include "uiDefines.h"

#ifdef USE_OPEN_WEATHER
#include "uiVars.h"

void openWeather_task(void *pvParameters) {
  
  vTaskSuspend(NULL);

  ESP_LOGI("openWeather manager: Task running on core %d", xPortGetCoreID());

  int errors = 0;

  for (;;) {
    myWiFi->ensureConnection();
    if (myWiFi->isConnected()) {    
      if (openWeather->fetchData()) {
        vTaskDelay(DELAY_OPEN_WEATHER_TASK / portTICK_PERIOD_MS);
      } else {
        vTaskDelay(DELAY_OPEN_WEATHER_SHORT_TASK / portTICK_PERIOD_MS);
      }
    } else {
      ESP_LOGE("No internet connection");
      vTaskDelay(DELAY_WIFI_RECONNECT_TASK / portTICK_PERIOD_MS);
    }
  }
}

#endif