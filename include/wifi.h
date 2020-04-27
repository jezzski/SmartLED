/** \file
 * Description: Contains function declarations to connect to a WiFi networking using SSID defined in wifi.cpp
 * \author: Lead: Andy Yeung, Secondary: Jesse Cannon
 * \date last modified: 4/26/2020
 */

#ifndef WIFI_H
#define WIFI_H
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#ifdef _cplusplus
extern "C" {
#endif
   static esp_err_t event_handler(void *ctx, system_event_t *event);
   void wifi_init_softap();
   void wifi_init_sta();
   
   
   #ifndef MIN
      #define MIN(x,y) ((x)<(y)?(x):(y))
   #endif
#ifdef _cplusplus
}
#endif
#endif