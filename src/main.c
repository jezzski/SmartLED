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

#include "lwip/err.h"
#include "lwip/sys.h"

#define HOME_PAGE "<html> <head> <title> This is the page title</title> </head> <body> Hello World <form action=""/echo"" method=""post"" id=""form1""> First name: <input type=""text"" name=""fname""><br> Last name: <input type=""text"" name=""lname""><br> </form> <button type=""submit"" form=""form1"" value=""Submit"">Submit</button></body></html>"
static const char * HTTP_TAG = "HTTP";

// Test Block
esp_err_t homepage_handler(httpd_req_t* req)
{
    ESP_LOGI(HTTP_TAG, "Reached homepage_handler");
    /* Set some custom headers */
    httpd_resp_set_hdr(req, "Custom-Header-1", "Custom-Value-1");
    httpd_resp_set_hdr(req, "Custom-Header-2", "Custom-Value-2");
    /* Send response with custom headers and body set as the
     * string passed in user context*/
    const char* resp_str = (const char*) req->user_ctx;
    FILE * fp;
    fp = fopen("/spiffs/index.html", "r");
    char str[60];
    if(fp == NULL){
        ESP_LOGI("SPIFF","Error Opening File");
    }
    else
    {
        fgets(str,60,fp);
    }
    
    httpd_resp_send(req, resp_str, strlen(resp_str));

    return ESP_OK;
}

esp_err_t echo_post_handler(httpd_req_t* req)
{
    char buf[100];
    int ret, remaining = req->content_len;

    while (remaining > 0) {
        /* Read the data for the request */
        if ((ret = httpd_req_recv(req, buf,
                        MIN(remaining, sizeof(buf)))) <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                /* Retry receiving if timeout occurred */
                continue;
            }
            return ESP_FAIL;
        }

        /* Send back the same data */
        httpd_resp_send_chunk(req, buf, ret);
        remaining -= ret;

        /* Log data received */
        ESP_LOGI(HTTP_TAG, "=========== RECEIVED DATA ==========");
        ESP_LOGI(HTTP_TAG, "%.*s", ret, buf);
        ESP_LOGI(HTTP_TAG, "====================================");
    }

    // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

// temp app_main for spiff testing
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
        // fgets(str,60,fp);
        // str[59] = (char) "\n";
        // ESP_LOGI("SPIFFS", "%s", str);
    }
    fclose(fp);
    size_t total = 0, used = 0;
    ret = esp_spiffs_info("website", &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE("SPIFFS", "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI("SPIFFS", "Partition size: total: %d, used: %d", total, used);
    }
}

// void app_main()
// {
//     //Initialize NVS
//     esp_err_t ret = nvs_flash_init();
//     if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
//       ESP_ERROR_CHECK(nvs_flash_erase());
//       ret = nvs_flash_init();
//     }
//     ESP_ERROR_CHECK(ret);

//     esp_vfs_spiffs_conf_t conf = {
//       .base_path = "/spiffs",
//       .partition_label = NULL, // defaults to first spiff subtype partition
//       .max_files = 5,
//       .format_if_mount_failed = true
//     };
    
//     // Use settings defined above to initialize and mount SPIFFS filesystem.
//     // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
//     ret = esp_vfs_spiffs_register(&conf);
//     if(ret == ESP_OK){
//         ESP_LOGI("SPIFFS","Successful mount");
//     }
//     ESP_LOGI(HTTP_TAG, "ESP_WIFI_MODE_STA");
//     wifi_init_sta();

//     /* Generate default configuration */
//     httpd_config_t config = {                           
//         .task_priority      = tskIDLE_PRIORITY+5,       
//         .stack_size         = 4096,                     
//         .server_port        = 80,                       
//         .ctrl_port          = 32768,                    
//         .max_open_sockets   = 7,                        
//         .max_uri_handlers   = 8,                        
//         .max_resp_headers   = 8,                        
//         .backlog_conn       = 5,                        
//         .lru_purge_enable   = false,                    
//         .recv_wait_timeout  = 5,                        
//         .send_wait_timeout  = 5,                        
//         .global_user_ctx = NULL,                        
//         .global_user_ctx_free_fn = NULL,                
//         .global_transport_ctx = NULL,                   
//         .global_transport_ctx_free_fn = NULL,           
//         .open_fn = NULL,                                
//         .close_fn = NULL,                               
//         .uri_match_fn = NULL                            
// }; //HTTPD_DEFAULT_CONFIG();

//     /* Empty handle to esp_http_server */
//     httpd_handle_t server = NULL;

//     /* make a sample uri */
//     static const httpd_uri_t home_page = {
//         .uri = "/",
//         .method = HTTP_GET,
//         .handler = homepage_handler,
//         .user_ctx = HOME_PAGE
//     };

//     static const httpd_uri_t echo = {
//         .uri = "/echo",
//         .method = HTTP_POST,
//         .handler = echo_post_handler,
//         .user_ctx = NULL
//     };

//     /* Start the httpd server */
//     if (httpd_start(&server, &config) == ESP_OK) {
//         /* Register URI handlers */
//         httpd_register_uri_handler(server, &home_page);
//         httpd_register_uri_handler(server, &echo);
//         //httpd_register_uri_handler(server, &uri_get);
//         //httpd_register_uri_handler(server, &uri_post);
//     }

// }