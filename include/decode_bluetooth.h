#ifndef DECOME_BLUETOOTH_H
#define DECODE_BLUETOOTH_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"

/**
 * @brief Looks for "Begin Read" from the packet recieved.
 * Saves channel number found in packet in saved_channel_num.
 * Calls set_schedule read, passing handle parameter
 * 
 * @param packet recieved byte packet
 * @param handle handle for the characteristic that will be read
 * @return esp_err_t 
 */
esp_err_t start_schedule_read(uint8_t* packet, uint16_t handle);
/**
 * @brief Sets GATT characteristic with the respective handle to the value of a schedule. 
 * First sets the characteristic to the name of the first schedule. 
 * On the next read it will set the characteristic to the schedule information.
 * This will repeat until there are no more schedules in the requested channel (saved_channel_num).
 * 
 * @param handle handle for the characteristic that will be read
 * @return esp_err_t 
 */
esp_err_t set_schedule_read(uint16_t handle);
/**
 * @brief Converts byte packet in order to create schedule object.
 * Recieves name from saved_name. 
 * 
 * @param packet recieved byte packet with schedule information
 * @return esp_err_t 
 */
esp_err_t decode_ble_schedule(uint8_t* packet);
/**
 * @brief Converts byte packet into string and saves value in saved_name.
 * 
 * @param packet recieved byte packet with scheudle name
 * @param length length of recieved packet
 * @return esp_err_t 
 */
esp_err_t decode_ble_schedule_name(uint8_t* packet, uint16_t length);
/**
 * @brief Converts byte packet into UNIX time value and calls set_time function
 * 
 * @param packet recieved byte packet with time information 
 * @return esp_err_t 
 */
esp_err_t decode_ble_time(uint8_t* packet);
/**
 * @brief Converts byte packet into channel, brightness, and color information.
 * If RGB channel is written, calls set_color function.
 * If normal LED channel is written, calls channel_on function.
 * 
 * @param packet recieved byte packet with led control information
 * @return esp_err_t 
 */
esp_err_t decode_ble_direct(uint8_t* packet);
/**
 * @brief Converts byte packet into schedule name and channel.
 * The schedule on the corresponding channel is then deleted using delete_schedule_by_name function.
 * 
 * @param packet recieved byte packet with schedule name to delete
 * @return esp_err_t 
 */
esp_err_t decode_ble_delete(uint8_t* packet);
/**
 * @brief Takes byte array and converts into a uint32 using the first 4 indices. Assumes Big-endian.
 * 
 * @param begin byte array
 * @return uint32_t 
 */
uint32_t get_Int32(uint8_t* begin);

#endif