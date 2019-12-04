/*
basic example used in PDR prototype demonstration
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "driver/ledc.h"

#include "wifi.h"
#include "espsntp.h"
#include "scheduler.h"
#include "led.h"
#include "bleSL.h"

void app_main()
{
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );

    Init_Bluetooth();
    //init gpio/LED outputs
    init_channels();
    //init schedule
    init_schedule();
    //init wifi
    //wifi_init_sta();
    //init sntp
    //obtain_time();
    
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    schedule_object s = {
        .ID = 0,
        .enabled = 1,
        .start = 0,
        .duration = 1,
        .repeat_mask = 0b00000000,
        .repeat_time = 7,

        .isRGB = 1, //could combine with a previous field to save memory space
        .brightness = 0xFF,
        .r = 0x00,
        .g = 0x00,
        .b = 0xFF,
    };
    strcpy(s.name, "Schedule1");
    //create_schedule(1, s);
    s.start = 60;
    s.duration = 1;
    s.repeat_time = 7;
    s.isRGB = 1;
    s.r = 0xFF;
    s.g = 0x00;
    s.b = 0x00;
    s.ID = 1;
    strcpy(s.name, "Schedule2");
    //create_schedule(1, s);


    uint8_t brightness = 0x00;
    while (1)
    {
        //brightness += 0x01;
        //channel_on(2, brightness);
        time_t curr;
        time(&curr);
        //printf("Current unix time:%ld\n", curr);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}