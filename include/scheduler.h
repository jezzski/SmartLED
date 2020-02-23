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
//todo: revisit with C++ style coding, would make certain actions way easier

void init_schedule(void);
//should make more of these that are easier to use?
esp_err_t create_schedule(uint8_t channel, schedule_object s);
esp_err_t delete_schedule_by_id(uint8_t channel, uint8_t ID);
esp_err_t delete_schedule_by_name(uint8_t channel, char *name);

//todo: integrate with LED control properly
#define NUM_CHANNELS 6
//List *schedules[NUM_CHANNELS];
extern List *schedules[NUM_CHANNELS];

#endif