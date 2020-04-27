/** \file
 * Description:
 * \author: Andy Yeung
 * \date: 4/26/2020
 */

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
    
    /**
     * @brief Initializes a http server with other handlers defined the file.
     * 
     * @param server A handle for the server.
     * @return esp_err_t Returns ESP_OK on successful deletion. ESP_ERR_INVALID_ARG if channel number is invalid. ESP_ERR_NOT_FOUND if schedule not found
     */
    esp_err_t init_http(httpd_handle_t server);
    /**
     * @brief Handler to retrieve home/direct control page. Accessed when server receives "/" or "/index.html".
     * 
     * @param req An incoming HTTP request.
     * @return esp_err_t Returns ESP_OK on successful deletion. ESP_ERR_INVALID_ARG if channel number is invalid. ESP_ERR_NOT_FOUND if schedule not found
     */
    esp_err_t homepage_handler(httpd_req_t* req);
    /**
     * @brief Handler to retrieve styles CSS page. Accessed when server receives "/styles.css".
     * 
     * @param req An incoming HTTP request.
     * @return esp_err_t Returns ESP_OK on successful deletion. ESP_ERR_INVALID_ARG if channel number is invalid. ESP_ERR_NOT_FOUND if schedule not found
     */
    esp_err_t styles_handler(httpd_req_t* req);
    /**
     * @brief Handler to retrieve schedules page. Accessed when server receives "/schedules.html"
     * 
     * @param req An incoming HTTP request.
     * @return esp_err_t Returns ESP_OK on successful deletion. ESP_ERR_INVALID_ARG if channel number is invalid. ESP_ERR_NOT_FOUND if schedule not found
     */
    esp_err_t schedules_handler(httpd_req_t* req);
    /**
     * @brief Handler to retrieve scripts page. Accessed when server receives "/schedules.js"
     * 
     * @param req An incoming HTTP request.
     * @return esp_err_t Returns ESP_OK on successful deletion. ESP_ERR_INVALID_ARG if channel number is invalid. ESP_ERR_NOT_FOUND if schedule not found
     */
    esp_err_t scripts_handler(httpd_req_t* req);
    /**
     * @brief Handler to create new schedule. Accessed when server receives or "/post_sch"
     * 
     * @param req An incoming HTTP request.
     * @return esp_err_t Returns ESP_OK on successful deletion. ESP_ERR_INVALID_ARG if channel number is invalid. ESP_ERR_NOT_FOUND if schedule not found
     */
    esp_err_t schedule_post_handler(httpd_req_t* req);
    /**
     * @brief Handler to retrieve favicon. Accessed when server receives "/favicon.ico" - DOES NOT WORK
     * 
     * @param req An incoming HTTP request.
     * @return esp_err_t Returns ESP_OK on successful deletion. ESP_ERR_INVALID_ARG if channel number is invalid. ESP_ERR_NOT_FOUND if schedule not found
     */
    esp_err_t favicon_ico_get_handler(httpd_req_t* req);
    /**
     * @brief Handler to post time to esp32. Accessed when server receives "/time"
     * 
     * @param req An incoming HTTP request.
     * @return esp_err_t Returns ESP_OK on successful deletion. ESP_ERR_INVALID_ARG if channel number is invalid. ESP_ERR_NOT_FOUND if schedule not found
     */
    esp_err_t time_post_handler(httpd_req_t* req);
    /**
     * @brief Handler for direct control. Access when server receives "/direct_control"
     * 
     * @param req An incoming HTTP request.
     * @return esp_err_t Returns ESP_OK on successful deletion. ESP_ERR_INVALID_ARG if channel number is invalid. ESP_ERR_NOT_FOUND if schedule not found
     */
    esp_err_t direct_control_post_handler(httpd_req_t* req);
    /**
     * @brief Handler to retrieve schedule data Access when server receives "/sch_data"
     * 
     * @param req An incoming HTTP request.
     * @return esp_err_t Returns ESP_OK on successful deletion. ESP_ERR_INVALID_ARG if channel number is invalid. ESP_ERR_NOT_FOUND if schedule not found
     */
    esp_err_t sch_data_post_handler(httpd_req_t* req);
    /**
     * @brief Function to process ";';"-delimited string. Parses out scheduing data and for schedule post handler
     * 
     * @param str Received string
     */
    void schTokenProcess(char* str);

}
#endif
