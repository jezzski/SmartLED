#ifndef HTTP_H
#define HTTP_H
extern "C" {
    #include <string.h>
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "freertos/event_groups.h"
    #include "esp_system.h"
    #include "esp_wifi.h"
    #include "esp_event_loop.h"
    #include "esp_log.h"
    #include "esp_http_server.h"
    
    static const char * HTTP_TAG = "HTTP";
    
    esp_err_t init_http(httpd_handle_t server);
    esp_err_t homepage_handler(httpd_req_t* req);
    esp_err_t styles_handler(httpd_req_t* req);
    esp_err_t schedules_handler(httpd_req_t* req);
    esp_err_t scripts_handler(httpd_req_t* req);
    esp_err_t schedule_post_handler(httpd_req_t* req);
    esp_err_t favicon_ico_get_handler(httpd_req_t* req);
    esp_err_t time_post_handler(httpd_req_t* req);
    esp_err_t direct_control_post_handler(httpd_req_t* req);
    void schTokenProcess(char* str);

}
#endif
