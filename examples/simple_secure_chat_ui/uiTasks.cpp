#include <Arduino.h>
#include "uiDefines.h"
#include "uiTasks.h"
#include "uiVars.h"

// Tasks
TaskHandle_t t_core0_lvgl;
TaskHandle_t t_core1_clock;  
#ifdef USE_OPEN_WEATHER
TaskHandle_t t_core1_openWeather;
#endif

void createTasks() {
  Serial.println("Creating Tasks...");

  xTaskCreatePinnedToCore(
    lvgl_task,       // Task function.
    "LVGL_Manager",  // Name of task.
    10000,          // Stack size of task
    NULL,           // Parameter of the task
    5,              // Priority of the task
    &t_core0_lvgl,  // Task handle to keep track of created task
    0);             // Pin task to core 0

  xTaskCreatePinnedToCore(
    clock_task,       // Task function.
    "CLOCK_Manager",  // Name of task.
    10000,          // Stack size of task
    NULL,           // Parameter of the task
    4,              // Priority of the task
    &t_core1_clock,  // Task handle to keep track of created task
    1);             // Pin task to core 0

  #ifdef USE_OPEN_WEATHER
  xTaskCreatePinnedToCore(
    openWeather_task,       // Task function.
    "OpenWeather_Manager",  // Name of task.
    10000,          // Stack size of task
    NULL,           // Parameter of the task
    3,              // Priority of the task
    &t_core1_openWeather,  // Task handle to keep track of created task
    1);             // Pin task to core 0

  #endif

  Serial.println("All tasks created\nStarting tasks...");

  vTaskResume(t_core0_lvgl);  
  vTaskResume(t_core1_clock);  

}
