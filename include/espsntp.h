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