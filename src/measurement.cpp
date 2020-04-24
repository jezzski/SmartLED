#include "driver/gpio.h"
#include "measurement.h"

#define TAG "OC"

uint8_t currentFault = 0;
uint8_t voltageFault = 0;

static void IRAM_ATTR current_isr_handler(void* arg)
{
    currentFault = 1;
    shutdown_outputs();
    //gpio_set_level(GPIO_NUM_22, 0);
}

uint8_t isCurrentFault()
{
    return currentFault;
}

uint8_t isVoltageFault()
{
    return voltageFault;
}

esp_err_t clearFaults()
{
    //steps:
    //1) reset current IC
        // there is some experimentation involved here, but need to do in certain order
        // for INA series, enable must be "reset" by bringing low and then bringing high again
        // may be required to reset limit as well, need to test with hardware
        
        //gpio_set_level(OC_ENABLE, 0);
        //wait x ms to be safe
        //gpio_set_level(OC_ENABLE, 255);
    //2) reset OV? depends on implementation chosen and additional hardware added, additional steps may be required
    //3) clear led shutdown
    if (clear_shutdown() != ESP_OK)
    {
        ESP_LOGE(TAG, "unable to clear faults: led's shutdown still enabled");
        return ESP_FAIL;
    }
    //4) reset fault vars
    voltageFault = 0;
    currentFault = 0;

    return ESP_OK;
}

esp_err_t set_current_level(double ampLimit)
{
    //for using DAC to control INA300
    //alert is active when voltage across shunt exceeds a certain limit
    //need the value of the shunt resistor
    //the desired trip voltage is Iload x Rsense
    double Rsense = 50/1000; //50 mOhms
    double trip = ampLimit * Rsense;
    if (trip > 3.3 || trip < 0)
    {
        ESP_LOGE(TAG, "set_current_level trip limit %lf is not valid for this device", trip);
        return ESP_FAIL;
    }
    //then convert a voltage value into esp32 equivalent with DAC
    //esp32 is 3.3V
    //so formula should be trip/VDD * (uint8_t max)
    uint8_t v_trip = (trip/3.3)*(255);

    //note: may be a good idea to temporarily disable the OC protection while the limit is being set
    //this would prevent any incorrect faults being detected as limit value changes

    esp_err_t ret = dac_output_voltage(DAC_CHANNEL_1, v_trip); //test output VDD voltage
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "error while setting dac output: %s", esp_err_to_name(ret));
        return ret;
    }

    return ESP_OK;
}

esp_err_t set_voltage_level(double voltLimit)
{
    //depends on implementation
    //if task, set a global variable to compare against periodically
    //dac returns different values based on resolution
    //voltage = (dac_return/MAX_POSSIBLE_VALUE)*3.3V
    //then need to calculate actual value from voltage divider values
    //then compare against value set by this function
    
    //if using interrupt with external hardware,
    //perform similar calculations for the level the hardware requires
    //then, set a dac value to the level calculated (see set_current_limit)

    return ESP_FAIL;
}

esp_err_t init_oc(void)
{
    ESP_LOGI(TAG, "Starting DAC");
    esp_err_t ret;
    //attempt to enable esp32's DAC
    ret = dac_output_enable(DAC_CHANNEL_1);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "error while enabling dac: %s", esp_err_to_name(ret));
        return ret;
    }

    double cLevel = 0; //current level
    double vLevel = 0; //voltage level

    //try to get current setting from memory
    ret = get_setting_double("current_level", &cLevel);
    if (ret != ESP_OK)
    {
        //could not find setting, default to 2A limit and store to memory
        cLevel = 2;
        store_setting_double("current_level", cLevel);
    }
    //try to get voltage setting from memory
    ret = get_setting_double("voltage_level", &vLevel);
    if (ret != ESP_OK)
    {
        //could not find setting, default to 24V limit and store to memory
        vLevel = 24;
        store_setting_double("voltage_level", vLevel);
    }

    //set limits to recovered/default values
    ret = set_current_level(cLevel);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "could not set current limit on init!");
        return ret;
    }
    set_voltage_level(vLevel);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "could not set voltage limit on init!");
        return ret;
    }
    
    
    //configure pins used for overcurrent chip
    
    //set up enable pin
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = 1ULL<<OC_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    //set up alert pin
    //io_conf.intr_type = GPIO_PIN_INTR_NEGEDGE;
    io_conf.pin_bit_mask = 1ULL<<OC_ALERT;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    gpio_config(&io_conf);

    //configure falling edge interrupt on alert pin (INA is active low)
    gpio_set_intr_type(OC_ALERT, GPIO_INTR_NEGEDGE);
    gpio_install_isr_service(0);
    //gpio_isr_register();
    gpio_isr_handler_add(OC_ALERT, current_isr_handler, NULL);

    //enable over current
    gpio_set_level(OC_ENABLE, 255);



    //Voltage here:
    //depends on implementation chosen
    //if using periodic sampling need a rtos task, see scheduler for example of setting up a task
    //if using a rising edge interrupt need to configure another interrupt
    return ESP_OK;
}