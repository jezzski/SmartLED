#ifndef SCHEDULE_H
#define SCHEDULE_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"

#include <time.h>
#include <string.h>

#include "schedule_object.h"
#include "led.h"

//todo: good c coding to make variables "private" when possible

void init_schedule(void);
esp_err_t create_schedule(uint8_t channel, schedule_object s);
//todo: schedule deletion
esp_err_t delete_schedule_by_id(uint8_t channel, uint8_t ID);
esp_err_t delete_schedule_by_name(uint8_t channel, char *name);

esp_err_t disable_schedule_by_id(uint8_t channel, uint8_t ID);
esp_err_t disable_schedule_by_name(uint8_t channel, char *name);

esp_err_t enable_schedule_by_id(uint8_t channel, uint8_t ID);
esp_err_t enable_schedule_by_name(uint8_t channel, char *name);

esp_err_t disable_all_schedules(void);
esp_err_t enable_all_schedules(void);
esp_err_t delete_all_schedules(void);

esp_err_t get_schedule_names(uint8_t channel, char* &out);
esp_err_t get_schedule(uint8_t channel, char *name, schedule_object *out);

//todo: integrate with LED control properly
extern List *schedules[NUM_CHANNELS];

#endif