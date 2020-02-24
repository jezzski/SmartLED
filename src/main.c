/* Simple WiFi Example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_http_server.h"
#include "esp_spiffs.h"

#include "wifi.h"
#include "http.h"
#include "led.h"
#include "scheduler.h"
#include "espsntp.h"

#include "lwip/err.h"
#include "lwip/sys.h"


// esp_err_t echo_post_handler(httpd_req_t* req)
// {
//     char buf[100];
//     int ret, remaining = req->content_len;

//     while (remaining > 0) {
//         /* Read the data for the request */
//         if ((ret = httpd_req_recv(req, buf,
//                         MIN(remaining, sizeof(buf)))) <= 0) {
//             if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
//                 /* Retry receiving if timeout occurred */
//                 continue;
//             }
//             return ESP_FAIL;
//         }

//         /* Send back the same data */
//         httpd_resp_send_chunk(req, buf, ret);
//         remaining -= ret;

//         /* Log data received */
//         ESP_LOGI(HTTP_TAG, "=========== RECEIVED DATA ==========");
//         ESP_LOGI(HTTP_TAG, "%.*s", ret, buf);
//         ESP_LOGI(HTTP_TAG, "====================================");
//     }

//     // End response
//     httpd_resp_send_chunk(req, NULL, 0);
//     return ESP_OK;
// }

void app_main(){
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

    //Initialize NVS
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    init_channels();
    init_schedule();

    wifi_init_sta();

    httpd_handle_t server = NULL;  // empty server handle
    init_http(server);

    // vTaskDelay(2000 / portTICK_PERIOD_MS);
    // schedule_object s = {
    //     .ID = 0,
    //     .enabled = 1,
    //     .start = 0,
    //     .duration = 1,
    //     .repeat_mask = 0b00000000,
    //     .repeat_time = 7,

    //     .isRGB = 1, //could combine with a previous field to save memory space
    //     .brightness = 0xFF,
    //     .r = 0x00,
    //     .g = 0x00,
    //     .b = 0xFF,
    // };
    // strcpy(s.name, "Schedule1");
    // create_schedule(1, s);

    while(1) vTaskDelay(1000/portTICK_PERIOD_MS);
}
