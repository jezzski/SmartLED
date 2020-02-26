#include "measurement.h"

#define TAG "OC"

void init_oc(void)
{
    //should this take current level now?

    ESP_LOGI(TAG, "Starting DAC");
    dac_output_enable(OC_LIMIT);

    uint8_t t = 0;
    while (1) {
        dac_output_voltage(OC_LIMIT, t++); //test output VDD voltage
         vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}