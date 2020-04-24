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

esp_err_t init_oc(void);

esp_err_t set_current_level(double ampLimit);

esp_err_t set_voltage_level(double voltLimit);

uint8_t isCurrentFault();
uint8_t isVoltageFault();

esp_err_t clearFaults();

#endif