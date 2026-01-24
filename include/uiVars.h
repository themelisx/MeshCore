#ifndef VARS_h
#define VARS_h

#include <Arduino.h>
#include "openWeather.h"
#include "lvgl.h"

extern void createTasks();

extern TaskHandle_t t_core1_tft;
extern TaskHandle_t t_core0_lvgl;
extern TaskHandle_t t_core1_clock;  

extern SemaphoreHandle_t semaphoreData;

extern UIManager *uiManager;
extern MyClock *myClock;
extern MyWiFi *myWiFi;

#ifdef USE_OPEN_WEATHER
    extern OpenWeather *openWeather;
    extern TaskHandle_t t_core1_openWeather;
#endif
#ifdef USE_MAIN_TAB_VIEW
    extern lv_obj_t * ui_MainTabView;
#endif

#endif