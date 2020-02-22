#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_http_server.h"

#include "http.h"

esp_err_t init_http(httpd_handle_t server)
{
    httpd_config_t config = {                           
    .task_priority      = tskIDLE_PRIORITY+5,       
    .stack_size         = 4096,                     
    .server_port        = 80,                       
    .ctrl_port          = 32768,                    
    .max_open_sockets   = 7,                        
    .max_uri_handlers   = 8,                        
    .max_resp_headers   = 8,                        
    .backlog_conn       = 5,                        
    .lru_purge_enable   = false,                    
    .recv_wait_timeout  = 5,                        
    .send_wait_timeout  = 5,                        
    .global_user_ctx = NULL,                        
    .global_user_ctx_free_fn = NULL,                
    .global_transport_ctx = NULL,                   
    .global_transport_ctx_free_fn = NULL,           
    .open_fn = NULL,                                
    .close_fn = NULL,                               
    .uri_match_fn = NULL                            
    }; //HTTPD_DEFAULT_CONFIG();

    static const httpd_uri_t home_page = {
        .uri = "/index.html",
        .method = HTTP_GET,
        .handler = homepage_handler,
        .user_ctx = NULL    
    };

    static const httpd_uri_t styles = {
        .uri = "/styles.css",
        .method = HTTP_GET,
        .handler = styles_handler,
        .user_ctx = NULL
    };

    static const httpd_uri_t schedules = {
        .uri = "/schedules.html",
        .method = HTTP_GET,
        .handler = schedules_handler,
        .user_ctx = NULL
    };

    static const httpd_uri_t scripts = {
        .uri = "/scripts.js",
        .method = HTTP_GET,
        .handler = scripts_handler,
        .user_ctx = NULL
    };

    /* Start the httpd server */
    if (httpd_start(&server, &config) == ESP_OK) {
        /* Register URI handlers */
        httpd_register_uri_handler(server, &home_page);
        //httpd_register_uri_handler(server, &echo);
        httpd_register_uri_handler(server, &styles);
        httpd_register_uri_handler(server, &schedules);
        httpd_register_uri_handler(server, &scripts);
        //httpd_register_uri_handler(server, &uri_get);
        //httpd_register_uri_handler(server, &uri_post);
        return ESP_OK;
    }
    return ESP_FAIL;  // is this the right return type?
}

esp_err_t homepage_handler(httpd_req_t* req)
{
    ESP_LOGI(HTTP_TAG, "Reached homepage_handler");
    /* Set some custom headers */
    // httpd_resp_set_hdr(req, "Custom-Header-1", "Custom-Value-1");
    // httpd_resp_set_hdr(req, "Custom-Header-2", "Custom-Value-2");
    FILE * fp;
    fp = fopen("/spiffs/index.html", "r");
    char str[255];
    if(fp == NULL){
        ESP_LOGI("SPIFF","Error Opening File");
    }
    else
    {
        while(fgets(str,254,fp)!=NULL){
            httpd_resp_send_chunk(req, str, strlen(str));
        }
        fclose(fp);
        httpd_resp_send_chunk(req, NULL, 0);  // indicates end
    }
    ESP_LOGI(HTTP_TAG, "Finished homepage_handler");
    return ESP_OK;
}

esp_err_t styles_handler(httpd_req_t* req)
{
    ESP_LOGI(HTTP_TAG, "Reached styles handler");
    
    FILE * fp;
    fp = fopen("/spiffs/styles.css", "r");
    char str[255];
    if(fp == NULL){
        ESP_LOGI("SPIFF","Error Opening File");
        httpd_resp_send_404(req);
    }
    else
    {
        httpd_resp_set_type(req, "text/css");
        while(fgets(str,254,fp)!=NULL){
            httpd_resp_send_chunk(req, str, strlen(str));
        }
        fclose(fp);
        httpd_resp_send_chunk(req, NULL, 0);  // indicates end
    }
    return ESP_OK;
}

esp_err_t schedules_handler(httpd_req_t* req)
{
    ESP_LOGI(HTTP_TAG, "Reached schedules handler");
    
    FILE * fp;
    fp = fopen("/spiffs/schedules.html", "r");
    char str[255];
    if(fp == NULL){
        ESP_LOGI("SPIFF","Error Opening File");
        httpd_resp_send_404(req);
    }
    else
    {
        httpd_resp_set_type(req, "text/html");
        while(fgets(str,254,fp)!=NULL){
            httpd_resp_send_chunk(req, str, strlen(str));
        }
        fclose(fp);
        httpd_resp_send_chunk(req, NULL, 0);  // indicates end
    }
    return ESP_OK;
}

esp_err_t scripts_handler(httpd_req_t* req)
{
    ESP_LOGI(HTTP_TAG, "Reached scripts_handler");
    
    FILE * fp;
    fp = fopen("/spiffs/scripts.js", "r");
    char str[255];
    if(fp == NULL){
        ESP_LOGI("SPIFF","Error Opening File");
        httpd_resp_send_404(req);
    }
    else
    {
        httpd_resp_set_type(req, "application/javascript");
        while(fgets(str,254,fp)!=NULL){
            httpd_resp_send_chunk(req, str, strlen(str));
        }
        fclose(fp);
        httpd_resp_send_chunk(req, NULL, 0);  // indicates end
    }
    return ESP_OK;
}