#ifndef TASKS_h
#define TASKS_h

#include <Arduino.h>
#include "defines.h"
#include "vars.h"

void lvgl_task(void *pvParameters);
//void clock_task(void *pvParameters);
#ifdef USE_OPEN_WEATHER
void openWeather_task(void *pvParameters);
#endif

#endif