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

//two methods
//measure with ADC periodically and calculate current from difference

//set up INA OC with DAC limit and have an interrupt on alert

void init_oc(void);

#endif