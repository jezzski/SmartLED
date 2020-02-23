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
#include "memory.h"

extern "C" {
    void app_main();
}

void app_main()
{
    /*
        some potential TODO's:
        Debug wrapper so we can put debug statement in code that will not be used in final release
        revisit all c-code to use C++ features (ie classes and stuff where appropriate, would clean up code a lot)
        performance analysis (some parts of our code may be unnecessarily slow and be optimized greatly, like file I/O)
        build a call-stack and count depth, would help to see max stack size and set appropriate sizes so no overflows
        build a timing table for freeRTOS tasks so we can optimize timings so tasks are not constantly interrupting each other
    */


    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );

    
    err = init_memory();
    //init wifi
    //wifi_init_sta();

    //obtain_time();

    init_schedule();

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    //todo: should this be done automatically when init_memory is called?
    //should this return extra information like # of schedules an 
    recall_schedules();

    /*schedule_object s;
    s.ID = 0;
    strcpy(s.name, "test");
    s.enabled = 1;
    s.start = 10;
    s.duration = 10;
    s.repeat_mask = 0;
    s.repeat_time = 0;
    s.isRGB = 0;
    s.brightness = 255;
    s.r = 0;
    s.g = 0;
    s.b = 0;

    //create_schedule(0, s);
    err = create_schedule((uint8_t)0, (schedule_object)s);*/
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    while (1)
    {
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        //todo: create task to auto save ever x seconds and on outage?
        store_schedules();
    }
}