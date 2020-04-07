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


/**
 * @brief Init the scheduler code. Starts the background task if not already started. If this is not called the schedules will not run.
 * 
 */
void init_schedule(void);

/**
 * @brief Create a schedule to associate with the specified channel. TODO: overwrite schedule if already exists (conflicting ID/NAME)
 * 
 * @param channel The channel that the schedule will be placed on.
 * @param s The schedule object that will be placed.
 * @return esp_err_t Returns ESP_OK on success. ESP_ERR_INVALID_ARG if channel number is invalid. ESP_ERR_NO_MEM if unable to allocate schedule.
 */
esp_err_t create_schedule(uint8_t channel, schedule_object s);

/**
 * @brief Delete a schedule by ID.
 * 
 * @param channel The channel the schedule is located on.
 * @param ID The ID of the schedule to delete.
 * @return esp_err_t Returns ESP_OK on successful deletion. ESP_ERR_INVALID_ARG if channel number is invalid. ESP_ERR_NOT_FOUND if schedule not found
 */
esp_err_t delete_schedule_by_id(uint8_t channel, uint8_t ID);

/**
 * @brief Delete a schedule by name.
 * 
 * @param channel The channel the schedule is located on.
 * @param name The name of the schedule to delete.
 * @return esp_err_t Returns ESP_OK on successful deletion. ESP_ERR_INVALID_ARG if channel number is invalid. ESP_ERR_NOT_FOUND if schedule not found
 */
esp_err_t delete_schedule_by_name(uint8_t channel, char *name);

/**
 * @brief Disable a schedule by ID. The schedule remains in the linked list but does not run or perform updates.
 * 
 * @param channel The channel the schedule is located on.
 * @param ID The name of the schedule to disable.
 * @return esp_err_t ESP_OK on successful disable. ESP_ERR_INVALID_ARG if channel number is invalid. ESP_ERR_NOT_FOUND if schedule not found
 */
esp_err_t disable_schedule_by_id(uint8_t channel, uint8_t ID);

/**
 * @brief Disable a schedule by name. The schedule remains in the linked list but does not run or perform updates.
 * 
 * @param channel The channel the schedule is located on.
 * @param name The name of the schedule to disable.
 * @return esp_err_t ESP_OK on successful disable. ESP_ERR_INVALID_ARG if channel number is invalid. ESP_ERR_NOT_FOUND if schedule not found
 */
esp_err_t disable_schedule_by_name(uint8_t channel, char *name);

/**
 * @brief Enable a schedule by ID. If the schedule was disabled, the schedule now runs and performs updates.
 * 
 * @param channel The channel the schedule is located on.
 * @param ID The ID of the schedule to enable.
 * @return esp_err_t ESP_OK on successful enable. ESP_ERR_INVALID_ARG if channel number is invalid. ESP_ERR_NOT_FOUND if schedule not found
 */
esp_err_t enable_schedule_by_id(uint8_t channel, uint8_t ID);

/**
 * @brief Enable a schedule by name. If the schedule was disabled, the schedule now runs and performs updates.
 * 
 * @param channel The channel the schedule is located on.
 * @param name The name of the schedule to enable.
 * @return esp_err_t ESP_OK on successful enable. ESP_ERR_INVALID_ARG if channel number is invalid. ESP_ERR_NOT_FOUND if schedule not found
 */
esp_err_t enable_schedule_by_name(uint8_t channel, char *name);

/**
 * @brief Disables all schedules on all channels. The schedules remain located in memory but no longer run or perform updates.
 * 
 * @return esp_err_t ESP_OK on successful disable.
 */
esp_err_t disable_all_schedules(void);

/**
 * @brief Enable all schedules on all channels. If the schedule was disabled, the schedule now runs and performs updates.
 * 
 * @return esp_err_t ESP_OK on successful disable.
 */
esp_err_t enable_all_schedules(void);

/**
 * @brief Delete all schedules on all channels.
 * 
 * @return esp_err_t ESP_OK on successful deletion.
 */
esp_err_t delete_all_schedules(void);

/**
 * @brief Get a json string of all schedules and their enabled status for a given channel.
 * 
 * @param channel The channel to get the schedule names and status from.
 * @param out The json string that is returned. Format is '{ "Name1":0, "Name2":1, "Name3":0 }'
 * @return esp_err_t ESP_OK if successful. ESP_ERR_INVALID_ARG if channel is invalid.
 */
esp_err_t get_schedule_names(uint8_t channel, char* &out);

/**
 * @brief Get the schedule object specified.
 * 
 * @param channel The channel the schedule is located on.
 * @param name The name of the schedule to find.
 * @param out The schedule returned. NULL if failure occurred.
 * @return esp_err_t ESP_OK if successful. ESP_ERR_INVALID_ARG if channel is invalid. ESP_ERR_NOT_FOUND if the schedule could not be found.
 */
esp_err_t get_schedule(uint8_t channel, char *name, schedule_object *out);

//todo: integrate with LED control properly
/**
 * @brief The linked list associated with each channel. Can be used directly instead of scheduler functions if so desired.
 * 
 */
extern List *schedules[NUM_CHANNELS];

#endif