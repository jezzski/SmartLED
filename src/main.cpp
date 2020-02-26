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
#include "memory.h"
#include "wifi.h"

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

   //todo: this shouldn't be in main, specific to website component
   //=====================================================================================================================
   esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = "website",
      .max_files = 10,
      .format_if_mount_failed = false
    };

    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if(ret == ESP_OK){
        ESP_LOGI("SPIFFS","Successful mount");
    }
    else if (ret == ESP_ERR_NOT_FOUND){
        ESP_LOGE("SPIFFS", "Failed to find SPIFFS partition");
    }

    FILE * fp;
    fp = fopen("/spiffs/settings.html", "r");
    // char str[60];
    if(fp == NULL){
        ESP_LOGI("SPIFFS","Error Opening File");
    }
    else
    {
        ESP_LOGI("SPIFFS", "Accessed the file!");
    }
    fclose(fp);
    size_t total = 0, used = 0;
    ret = esp_spiffs_info("website", &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE("SPIFFS", "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI("SPIFFS", "Partition size: total: %d, used: %d", total, used);
    }
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
    err = init_memory();
    init_schedule();
    //wifi_init_sta();


    vTaskDelay(1000 / portTICK_PERIOD_MS);
    //todo: should this be done automatically when init_memory is called?
    //should this return extra information like # of schedules an 
    recall_schedules();
    httpd_handle_t server = NULL;  // empty server handle
    //init_http(server);

    uint8_t brightness = 0;
    while (1)
    {
        vTaskDelay(200 / portTICK_PERIOD_MS);
        printf("Test:%d\n", brightness);
        //channel_on(0, brightness);
        set_color(0, 255, 0, 0, brightness);
        brightness += 5;
    }

    /*while (1)
    {
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        //todo: create task to auto save ever x seconds and on outage?
        store_schedules();
    }*/
}