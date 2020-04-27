/** \file
 * Description: Contains function declarations for OC/OV subsystem. Intended to set levels and take actions for overcurrent and overvoltage conditions.
 * NOTE: this system is basically just an outline. Development was not completed b/c of a lack of hardware and time constraints.
 * \author: Jesse Cannon
 * \date last modified: 4/26/2020
 */

#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include "driver/gpio.h"
#include "driver/adc.h"
#include "driver/dac.h"
#include "esp_system.h"
#include "esp_log.h"
#include "pin_defs.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "led.h"
#include "memory.h"

//two methods
//measure with ADC periodically and calculate current from difference

//set up INA OC with DAC limit and have an interrupt on alert

/**
 * @brief Inits the overcurrent and overvoltage detection systems. The previously used values will be set as the level, with defaults of 24 Volts and 2 Amps triggering faults.
 * 
 * @return esp_err_t 
 */
esp_err_t init_oc(void);

/**
 * @brief Set the current level that will trigger a fault.
 * 
 * @param ampLimit The current level to be used to determine faults, such as 1.1 Amps.
 * @return esp_err_t ESP_OK if the new level was successfully set, else if error occurred.
 */
esp_err_t set_current_level(double ampLimit);

/**
 * @brief Set the voltage level that will trigger a fault.
 * 
 * @param voltLimit The voltage level to be used to determine faults, such as 12.5 Volts.
 * @return esp_err_t ESP_OK if the new level was successfully set, else if error occurred.
 */
esp_err_t set_voltage_level(double voltLimit);

/**
 * @brief Determines if the device is in shutdown from an overcurrent condition.
 * 
 * @return uint8_t A value of 1 means that there is currently a fault. A value of 0 means there is no fault
 */
uint8_t isCurrentFault();

/**
 * @brief Determines if the device is in shutdown from an overvoltage condition.
 * 
 * @return uint8_t A value of 1 means that there is currently a fault. A value of 0 means there is no fault
 */
uint8_t isVoltageFault();

/**
 * @brief Clears all faults from the system and allows outputs to be controlled again.
 * 
 * @return esp_err_t ESP_OK if successfully re-enables outputs. ESP_FAIL if error occurs.
 */
esp_err_t clearFaults();

#endif