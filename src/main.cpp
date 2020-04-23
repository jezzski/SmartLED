/*
example used in alpha demonstration
*/

//C libraries
#include <stdio.h>
#include <string.h>

//esp32 & related libraries
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event_loop.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"

//custom libraries
#include "espsntp.h"
#include "http.h"
#include "led.h"
#include "scheduler.h"
#include "measurement.h"
#include "memory.h"
#include "wifi.h"
#include "bleSL.h"
#include "decode_bluetooth.h"

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

    //=====================================================================================================================

    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );

    //init functions
    //todo: error checking
    init_channels();
    init_oc();
    err = init_memory();
    init_schedule();    
    wifi_init_sta();

    vTaskDelay(10000 / portTICK_PERIOD_MS);

    /*schedule_object s = { };
    s.ID = 0;
    strcpy(s.name, "test");
    create_schedule(0, s);*/

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    //todo: should this be done automatically when init_memory is called?
    //should this return extra information like # of schedules an 
    recall_schedules();
    httpd_handle_t server = NULL;  // empty server handle
    init_http(server);
    Init_Bluetooth();

    while (1)
    {
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        //todo: create task to auto save ever x seconds and on outage?
        store_schedules();
    }
}