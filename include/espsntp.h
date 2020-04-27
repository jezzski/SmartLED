/** \file
 * Description: Contains function declarations to start the process of obtaining time from an SNTP server.
 * NOTE: not tested with latest version of project. Was initially developed very early in project and abandoned for other priorites. 
 * \author: Jesse Cannon (based on https://github.com/espressif/esp-idf/blob/5aa21584cfb4cfe4f2c7f6250151341e28141cf3/examples/protocols/sntp/main/sntp_example_main.c)
 * \date last modified: 4/26/2020
 */

#ifndef ESPSNTP_H
#define ESPSNTP_H

#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "lwip/apps/sntp.h"

void obtain_time(void);
void set_time(uint32_t time);
//todo: sntp with old version of idf or upgrade to 4.0 somehow?
#endif
