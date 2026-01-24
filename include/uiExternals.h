#ifndef EXTERNALS_h
#define EXTERNALS_h

#include <Arduino.h>

#include "uiConfiguration.h"

extern void createTasks();
extern TaskHandle_t t_core0_lvgl;
extern TaskHandle_t t_core1_clock;  
#ifdef USE_OPEN_WEATHER
    extern TaskHandle_t t_core1_openWeather;
#endif

#endif