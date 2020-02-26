#ifndef MEMORY_H
#define MEMORY_H

//external libraries
#include "ArduinoJson-v6.14.1.h"

//c libraries
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <dirent.h>

//esp32 libraries
#include "main.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"

//project libraries
#include "scheduler.h"

esp_err_t init_memory(void);

esp_err_t store_schedules(void);
esp_err_t recall_schedules(void);
esp_err_t clear_schedule_data(void);

#endif