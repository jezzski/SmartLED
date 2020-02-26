#include "driver/gpio.h"
#include "measurement.h"

#define TAG "OC"

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    gpio_set_level(GPIO_NUM_22, 0);
}

void init_oc(void)
{
    //should this take current level now?

    ESP_LOGI(TAG, "Starting DAC");
    dac_output_enable(DAC_CHANNEL_1);
    dac_output_voltage(DAC_CHANNEL_1, 255); //test output VDD voltage


    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = 1ULL<<GPIO_NUM_22;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    //io_conf.intr_type = GPIO_PIN_INTR_NEGEDGE;
    io_conf.pin_bit_mask = 1ULL<<GPIO_NUM_26;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    gpio_config(&io_conf);

    gpio_set_intr_type(GPIO_NUM_26, GPIO_INTR_NEGEDGE);
    gpio_install_isr_service(0);
    //gpio_isr_register();
    gpio_isr_handler_add(GPIO_NUM_26, gpio_isr_handler, NULL);

    gpio_set_level(GPIO_NUM_22, 255);

}