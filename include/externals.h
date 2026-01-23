#ifndef EXTERNALS_h
#define EXTERNALS_h

#include <Arduino.h>

#include "configuration.h"

extern void createTasks();
extern TaskHandle_t t_core1_lvgl;
//TaskHandle_t t_core1_clock;  
#ifdef USE_OPEN_WEATHER
    extern TaskHandle_t t_core1_openWeather;
#endif

#endif