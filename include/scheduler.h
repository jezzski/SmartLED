#ifndef SCHEDULE_H
#define SCHEDULE_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"

#include <time.h>
#include <string.h>

#include "schedule_object.h"

//todo: good c coding to make variables "private" when possible

void init_schedule(void);

//todo: integrate with LED control properly
#define NUM_CHANNELS 6
List *schedules[NUM_CHANNELS];

#endif