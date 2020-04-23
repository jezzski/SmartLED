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
#include "scheduler.h"
#include "espsntp.h"

esp_err_t init_http(httpd_handle_t server)
{
    httpd_config_t config = {                           
    .task_priority      = tskIDLE_PRIORITY+5,       
    .stack_size         = 4096,                     
    .server_port        = 80,                       
    .ctrl_port          = 32768,                    
    .max_open_sockets   = 7,                        
    .max_uri_handlers   = 10,                        
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

    static const httpd_uri_t home_page_get = {
        .uri = "/index.html",
        .method = HTTP_GET,
        .handler = homepage_handler,
        .user_ctx = NULL    
    };

    static const httpd_uri_t alt_home_page_get = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = homepage_handler,
        .user_ctx = NULL    
    };

    static const httpd_uri_t styles_get = {
        .uri = "/styles.css",
        .method = HTTP_GET,
        .handler = styles_handler,
        .user_ctx = NULL
    };

    static const httpd_uri_t schedules_get = {
        .uri = "/schedules.html",
        .method = HTTP_GET,
        .handler = schedules_handler,
        .user_ctx = NULL
    };

    static const httpd_uri_t scripts_get = {
        .uri = "/scripts.js",
        .method = HTTP_GET,
        .handler = scripts_handler,
        .user_ctx = NULL
    };

    static const httpd_uri_t schedule_post = {
        .uri = "/post_sch",
        .method = HTTP_POST,
        .handler = schedule_post_handler,
        .user_ctx = NULL
    };

    static const httpd_uri_t favicon_ico_get ={
        .uri = "/favicon.ico",
        .method = HTTP_GET,
        .handler = favicon_ico_get_handler,
        .user_ctx = NULL
    };

    static const httpd_uri_t time_post ={
        .uri = "/time",
        .method = HTTP_POST,
        .handler = time_post_handler,
        .user_ctx = NULL
    };

    static const httpd_uri_t direct_control_post ={
        .uri = "/direct_control",
        .method = HTTP_POST,
        .handler = direct_control_post_handler,
        .user_ctx = NULL
    };

    static const httpd_uri_t sch_data_post ={
        .uri = "/sch_data",
        .method = HTTP_POST,
        .handler = sch_data_post_handler,
        .user_ctx = NULL
    };

    /* Start the httpd server */
    if (httpd_start(&server, &config) == ESP_OK) {
        /* Register URI handlers */
        httpd_register_uri_handler(server, &home_page_get);
        httpd_register_uri_handler(server, &alt_home_page_get);
        httpd_register_uri_handler(server, &styles_get);
        httpd_register_uri_handler(server, &schedules_get);
        httpd_register_uri_handler(server, &scripts_get);
        httpd_register_uri_handler(server, &schedule_post);
        httpd_register_uri_handler(server, &favicon_ico_get);
        httpd_register_uri_handler(server, &time_post);
        httpd_register_uri_handler(server, &direct_control_post);
        httpd_register_uri_handler(server, &sch_data_post);

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
    ESP_LOGI(HTTP_TAG, "Finished styles_handler");
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
    ESP_LOGI(HTTP_TAG, "Finished schedules_handler");
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
        ESP_LOGI(HTTP_TAG, "Finished scripts_handler");
    }
    return ESP_OK;
}

esp_err_t schedule_post_handler(httpd_req_t* req){
    const char* SCHEDULE_POST_TAG = "HTTP-POST";
    ESP_LOGI(SCHEDULE_POST_TAG, "Reached schedule post handler");
    char buf[255];
    httpd_req_recv(req, buf, sizeof(buf));
    buf[254]=NULL;
    ESP_LOGI(SCHEDULE_POST_TAG, "%s", buf);
    schTokenProcess(buf);
    const char resp[] = "URI POST Response";
    httpd_resp_send(req, resp, strlen(resp));
    return ESP_OK;
}

#define DELIMITER (";';")

void schTokenProcess(char* str){
    const char* TOKEN_TAG = "TOKEN";
    ESP_LOGI(TOKEN_TAG, "Reached token process");
    static uint8_t prev_id = 0;  // should get rid of this var

    // Params needed for create_schedule
    uint8_t channel;
    uint8_t ID;
    char name[250];
    uint8_t enabled;
    uint32_t start;
    uint32_t duration;
    uint8_t repeat_mask;
    uint32_t repeat_time;

    uint8_t isRGB;
    uint8_t brightness;
    uint8_t r;
    uint8_t g;
    uint8_t b;

    // variable to hold various tokens before conversion to 
    char* token;

    // using strtok to parse http-post data
    // order of data is known
    // 0 - index
    token = strtok(str, DELIMITER);
    strcpy(name, token);
    ESP_LOGI(TOKEN_TAG, "Name: %s", name);
    // 1
    token = strtok(NULL, DELIMITER);
    ESP_LOGI(TOKEN_TAG, "Processing: %s", token);
    channel = (uint8_t) atoi(token);
    ESP_LOGI(TOKEN_TAG, "Channel: %u", channel);
    // 2
    token = strtok(NULL, DELIMITER);
    ESP_LOGI(TOKEN_TAG, "Processing: %s", token);
    if(strcmp(token,"true")==0) enabled=1;
    else enabled=0;
    ESP_LOGI(TOKEN_TAG, "Enabled: %u", enabled);
    // 3
    token = strtok(NULL, DELIMITER);
    ESP_LOGI(TOKEN_TAG, "Processing: %s", token);
    start = (uint32_t) atoi(token);
    ESP_LOGI(TOKEN_TAG, "Start: %u", start);
    // 4
    token = strtok(NULL, DELIMITER);
    ESP_LOGI(TOKEN_TAG, "Processing: %s", token);
    duration = (uint32_t) atoi(token);
    ESP_LOGI(TOKEN_TAG, "Duration: %u", duration);
    // 5
    token = strtok(NULL, DELIMITER);
    ESP_LOGI(TOKEN_TAG, "Processing: %s", token);
    repeat_time = (uint32_t) atoi(token);
    ESP_LOGI(TOKEN_TAG, "Repeat time: %u", repeat_time);
    // 6
    token = strtok(NULL, DELIMITER);
    ESP_LOGI(TOKEN_TAG, "Processing: %s", token);
    repeat_mask = (uint8_t) atoi(token);
    ESP_LOGI(TOKEN_TAG, "Repeat mask: %u", repeat_mask);
    // 7
    token = strtok(NULL, DELIMITER);
    ESP_LOGI(TOKEN_TAG, "Processing: %s", token);
    brightness = (uint8_t) atoi(token);
    ESP_LOGI(TOKEN_TAG, "Brightness: %u", brightness);
    // 8
    token = strtok(NULL, DELIMITER);
    ESP_LOGI(TOKEN_TAG, "Processing: %s", token);
    if(strcmp(token,"true")==0) isRGB=1;
    else isRGB=0;
    ESP_LOGI(TOKEN_TAG, "isRGB: %u", isRGB);
    // 9 - Color
    token = strtok(NULL, DELIMITER);
    ESP_LOGI(TOKEN_TAG, "Processing: %s", token);
    uint32_t temp = (uint32_t) strtol(token+1, NULL, 16);  // skip first char
    b = (uint8_t) temp;
    temp = temp >> 8;
    g = (uint8_t) temp;
    temp = temp >> 8;
    r = (uint8_t) temp;
    ESP_LOGI(TOKEN_TAG, "r: %X", r);
    ESP_LOGI(TOKEN_TAG, "g: %X", g);
    ESP_LOGI(TOKEN_TAG, "b: %X", b);

    schedule_object s;
    s.ID = prev_id++;
    strcpy(s.name, name);
    s.enabled = enabled;
    s.start = start;
    s.duration = duration;
    s.repeat_mask = repeat_mask;
    s.repeat_time = repeat_time;
    s.isRGB = isRGB;
    s.brightness = brightness;
    s.r = r;
    s.g = g;
    s.b = b;

    create_schedule(channel, s);

    ESP_LOGI(TOKEN_TAG, "Leave token processing");
}

esp_err_t favicon_ico_get_handler(httpd_req_t* req){
    ESP_LOGI(HTTP_TAG, "Reached favicon_ico_handler");
    
    FILE * fp;
    fp = fopen("/spiffs/favicon.ico", "r");
    char str[255];
    if(fp == NULL){
        ESP_LOGI("SPIFF","Error Opening File");
        httpd_resp_send_404(req);
    }
    else
    {
        httpd_resp_set_type(req, "image/x-icon");
        while(fgets(str,254,fp)!=NULL){
            httpd_resp_send_chunk(req, str, strlen(str));
        }
        fclose(fp);
        httpd_resp_send_chunk(req, NULL, 0);  // indicates end
        ESP_LOGI(HTTP_TAG, "Finished favicon_ico_handler");
    }
    return ESP_OK;
}

esp_err_t time_post_handler(httpd_req_t* req){
    const char* TIME_TAG = "HTTP-TIME";
    ESP_LOGI(TIME_TAG, "Reached time_post_handler");
    char buf[255];
    httpd_req_recv(req, buf, sizeof(buf));
    ESP_LOGI(TIME_TAG, "Time Received: %s", buf);
    uint32_t time = (uint32_t) atoi(buf);
    set_time(time);
    ESP_LOGI(TIME_TAG, "Finished time_post_handler");
    return ESP_OK;
}

esp_err_t direct_control_post_handler(httpd_req_t* req){
    const char* HTTP_TAG = "HTTP-Direct Control";
    ESP_LOGI(HTTP_TAG, "Reached direct_control_post_handler");

    char buf[255];
    httpd_req_recv(req, buf, sizeof(buf));
    buf[254]=NULL;
    ESP_LOGI(HTTP_TAG, "%s", buf);

    char cmd[16];

    // variable to hold various tokens before conversion to 
    char* token;
    token = strtok(buf, DELIMITER);
    strcpy(cmd,token);

    uint8_t channel;  // next token will be channel num
    token = strtok(NULL, DELIMITER);
    channel = (uint8_t) atoi(token);

    ESP_LOGI(HTTP_TAG, "%s : %u", cmd, channel);

    uint8_t brightness;

    if(!strcmp(cmd,"OnOff")){
        token = strtok(NULL, DELIMITER);
        brightness = (uint8_t) atoi(token);
        if(brightness) channel_on(channel, 100);
        else channel_off(channel);
        ESP_LOGI(HTTP_TAG, "On/Off %s", token);
    }
    else if(!strcmp(cmd,"Brightness")){
        token = strtok(NULL, DELIMITER);
        brightness = (uint8_t) atoi(token);
        channel_on(channel, brightness);
        ESP_LOGI(HTTP_TAG, "Brightness %s", token);
    }
    else if(!strcmp(cmd,"Color")){
        uint8_t r;
        uint8_t g;
        uint8_t b;
        token = strtok(NULL, DELIMITER);
        uint32_t temp = (uint32_t) strtol(token+1, NULL, 16);  // skip first char
        b = (uint8_t) temp;
        temp = temp >> 8;
        g = (uint8_t) temp;
        temp = temp >> 8;
        r = (uint8_t) temp;
        set_color(channel, r, g, b, 200);
        ESP_LOGI(HTTP_TAG, "Color %s", token);
    }
    else{  // should never hit unless trans error
        return ESP_FAIL;
    }

    const char resp[] = "URI POST Response";
    httpd_resp_send(req, resp, strlen(resp));

    ESP_LOGI(HTTP_TAG, "Finished direct_control_post_handler");
    return ESP_OK;
}

esp_err_t sch_data_post_handler(httpd_req_t* req){
    const char* HTTP_TAG = "HTTP-Sch Data";
    ESP_LOGI(HTTP_TAG, "Reached sch_data_post_handler");

    char buf[255];
    httpd_req_recv(req, buf, sizeof(buf));
    buf[254]=NULL;
    ESP_LOGI(HTTP_TAG, "%s", buf);

    char cmd[16];

    // variable to hold various tokens before conversion to 
    char* token;
    token = strtok(buf, DELIMITER);
    strcpy(cmd,token);

    uint8_t channel;
    if(!strcmp(cmd,"NamesList")){  // want list of schnames
        char* list;
        token = strtok(NULL, DELIMITER);
        channel = (uint8_t) atoi(token);
        get_schedule_names(channel, list);
        ESP_LOGI(HTTP_TAG, "%s", list);
        ESP_LOGI(HTTP_TAG, "%s", token);
        ESP_LOGI(HTTP_TAG, "%d", channel);
        httpd_resp_send(req, list, strlen(list));
        free(list);
    }
    else if(!strcmp(cmd,"SchData")){  // want sch data
        Schedule_Object sch_data;
        token = strtok(NULL, DELIMITER);
        channel = (uint8_t) atoi(token);
        token = strtok(NULL, DELIMITER);
        if(get_schedule(channel, token, &sch_data)==ESP_OK){
            char delimit[] = ";";
            char resp[256];  // prob overkill
            // TBD find more efficent way of transmitting
            char num_str[64];
            
            strcpy(resp, sch_data.name);  // sch name
            strcat(resp, delimit);

            sprintf(num_str, "%d", sch_data.start);
            strcat(resp, num_str);  // start time
            strcat(resp, delimit);

            sprintf(num_str, "%d", sch_data.duration);
            strcat(resp, num_str);  // duration
            strcat(resp, delimit);

            sprintf(num_str, "%d", sch_data.repeat_time);
            strcat(resp, num_str);  // repeat time
            strcat(resp, delimit);

            sprintf(num_str, "%d", sch_data.repeat_mask);
            strcat(resp, num_str);  // repeat mask
            strcat(resp, delimit);

            sprintf(num_str, "%d", sch_data.brightness);
            strcat(resp, num_str);  // brightness
            strcat(resp, delimit);

            sprintf(num_str, "%d", sch_data.isRGB);
            strcat(resp, num_str);  // RGB bool
            strcat(resp, delimit);

            sprintf(num_str, "%d", sch_data.r);
            strcat(resp, num_str);  // r
            strcat(resp, delimit);

            sprintf(num_str, "%d", sch_data.g);
            strcat(resp, num_str);  // g
            strcat(resp, delimit);

            sprintf(num_str, "%d", sch_data.b);
            strcat(resp, num_str);  // b
            strcat(resp, delimit);

            ESP_LOGI(HTTP_TAG, "%s", sch_data.name);
            ESP_LOGI(HTTP_TAG, "%s", token);
            ESP_LOGI(HTTP_TAG, "%d", channel);
            httpd_resp_send(req, resp, strlen(resp));
        }
        else{
            const char not_found[] = "Error: Did not find sch";
            httpd_resp_send(req, not_found, strlen(not_found));
        }
    }
    else if(!strcmp(cmd,"DelSch")){  // want to delete
        ESP_LOGI(HTTP_TAG, "Deleting schedule");
        token = strtok(NULL, DELIMITER);
        channel = (uint8_t) atoi(token);
        token = strtok(NULL, DELIMITER);
        delete_schedule_by_name(channel, token);
        ESP_LOGI(HTTP_TAG, "%s", token);
        ESP_LOGI(HTTP_TAG, "%d", channel);
        const char del[] = "Deleted schedule bu name";
        httpd_resp_send(req, del, strlen(del));
    }
    else{
        const char resp[] = "Error: Unable To Access Sch List";
        httpd_resp_send(req, resp, strlen(resp));
    }

    ESP_LOGI(HTTP_TAG, "Finished sch_data_post_handler");
    return ESP_OK;
}