/** \file
 * Description: Contains function declarations and settings used by the memory subsystem. Functions are primarily used by other files to get and retrieve values from persistent memory.
 * \author: Jesse Cannon
 * \date last modified: 4/26/2020
 */

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

/**
 * @brief The size of the buffer used to store settings. Increase this if storing a lot of settings
 * 
 */
#define SETTINGS_BUFFER_SIZE 512

/**
 * @brief Starts the memory system. Call this before calling any other memory related functions.
 * 
 * @return esp_err_t Returns ESP_OK on success. ESP_Fail or related if an error occurred.
 */
esp_err_t init_memory(void);

/**
 * @brief Stores all currently running schedules in persistent memory.
 * 
 * @return esp_err_t Returns ESP_OK on success. ESP_Fail or related if an error occurred.
 */
esp_err_t store_schedules(void);

/**
 * @brief Recall schedules from persistent memory. All schedules recalled will now be located in the tespective channel's schedule list.
 * 
 * @return esp_err_t Returns ESP_OK on success. ESP_Fail or related if an error occurred.
 */
esp_err_t recall_schedules(void);

/**
 * @brief Clears all schedules from persistent memory. Does not delete the schedules from the list.
 * 
 * @return esp_err_t Returns ESP_OK on success. ESP_Fail or related if an error occurred.
 */
esp_err_t clear_schedule_data(void);

/**
 * @brief Persistently store a string setting.
 * 
 * @param name The name of the setting to store. If the setting with that name already exists it is overwritten with the new value.
 * @param setting The setting to associate with the given name.
 * @return esp_err_t Returns ESP_OK on success. ESP_Fail or related if an error occurred.
 */
esp_err_t store_setting_string(const char *name, char *setting);

/**
 * @brief Persistently store an integer setting.
 * 
 * @param name The name of the setting to store. If the setting with that name already exists it is overwritten with the new value.
 * @param setting The setting to associate with the given name.
 * @return esp_err_t Returns ESP_OK on success. ESP_Fail or related if an error occurred.
 */
esp_err_t store_setting_int(const char *name, int setting);

/**
 * @brief Persistently store a byte setting.
 * 
 * @param name The name of the setting to store. If the setting with that name already exists it is overwritten with the new value.
 * @param setting The setting to associate with the given name. Uint8_t is equivalent to an unsigned char.
 * @return esp_err_t Returns ESP_OK on success. ESP_Fail or related if an error occurred.
 */
esp_err_t store_setting_byte(const char *name, uint8_t setting);

/**
 * @brief Persistently store a double setting.
 * 
 * @param name The name of the setting to store. If the setting with that name already exists it is overwritten with the new value.
 * @param setting The setting to associate with the given name. Exact storage depends on ArduinoJson configuration, but typically a maximum of 9 decimal places with rounding. No trailing zeros are included
 * @return esp_err_t Returns ESP_OK on success. ESP_Fail or related if an error occurred.
 */
esp_err_t store_setting_double(const char *name, double setting);

/**
 * @brief Recall a persistent string setting.
 * 
 * @param name The name of the setting to recall.
 * @param setting The output of that setting. Set to NULL if not found, the value if found.
 * @return esp_err_t Returns ESP_OK on success. ESP_Fail if not found or an error occurred.
 */
esp_err_t get_setting_string(const char *name, char *setting);

/**
 * @brief Recall a persistent integer setting.
 * 
 * @param name The name of the setting to recall.
 * @param setting The output of that setting. Set to NULL if not found, the value if found.
 * @return esp_err_t Returns ESP_OK on success. ESP_Fail if not found or an error occurred.
 */
esp_err_t get_setting_int(const char *name, int *setting);

/**
 * @brief Recall a byte string setting. Uint8_t is equivalent to unsigned char.
 * 
 * @param name The name of the setting to recall.
 * @param setting The output of that setting. Set to NULL if not found, the value if found.
 * @return esp_err_t Returns ESP_OK on success. ESP_Fail if not found or an error occurred.
 */
esp_err_t get_setting_byte(const char *name, uint8_t *setting);

/**
 * @brief Recall a persistent double setting. Exact storage depends on ArduinoJson configuration, but typically a maximum of 9 decimal places with rounding. No trailing zeros are included
 * 
 * @param name The name of the setting to recall.
 * @param setting The output of that setting. Set to NULL if not found, the value if found.
 * @return esp_err_t Returns ESP_OK on success. ESP_Fail if not found or an error occurred.
 */
esp_err_t get_setting_double(const char *name, double *setting);

/**
 * @brief Clear settings from persistent memory and RAM.
 * 
 * @return esp_err_t ESP_OK on success. ESP_FAIL if error occurrs.
 */
esp_err_t clear_setting_data(void);

#endif