#ifndef DECOME_BLUETOOTH_H
#define DECODE_BLUETOOTH_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"

esp_err_t decode_ble_schedule(uint8_t* start);
uint32_t get_Int32(uint8_t* begin);

#endif