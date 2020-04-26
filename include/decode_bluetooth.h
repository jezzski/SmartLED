#ifndef DECOME_BLUETOOTH_H
#define DECODE_BLUETOOTH_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"

esp_err_t start_schedule_read(uint8_t* packet, uint16_t handle);
esp_err_t set_schedule_read(uint16_t handle);
esp_err_t decode_ble_schedule(uint8_t* start);
esp_err_t decode_ble_time(uint8_t* start);
esp_err_t decode_ble_direct(uint8_t* start);
esp_err_t decode_ble_delete(uint8_t* start);
esp_err_t decode_ble_schedule_name(uint8_t* start, uint16_t length);
uint32_t get_Int32(uint8_t* begin);

#endif