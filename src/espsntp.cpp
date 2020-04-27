/** \file
 * Description: Contains function definitions to start the process of obtaining time from an SNTP server.
 * NOTE: not tested with latest version of project. Was initially developed very early in project and abandoned for other priorites. 
 * \author: Jesse Cannon (based on https://github.com/espressif/esp-idf/blob/5aa21584cfb4cfe4f2c7f6250151341e28141cf3/examples/protocols/sntp/main/sntp_example_main.c)
 * \date last modified: 4/26/2020
 */

#include "espsntp.h"

static const char *SNTP_TAG = "SNTP";

const int CONNECTED_BIT = BIT0;

static void initialize_sntp(void);
//static esp_err_t event_handler(void *ctx, system_event_t *event);

void set_time(uint32_t time)
{
    time_t now = time;
    struct timeval val;
    val.tv_sec = time;
    val.tv_usec = 0;
    settimeofday(&val, NULL);
    // Set timezone to Eastern Standard Time and print local time
    char strftime_buf[64];
    setenv("TZ", "EST5EDT,M3.2.0/2,M11.1.0", 1);
    tzset();
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    ESP_LOGI(SNTP_TAG, "The current date/time in New York is: %s", strftime_buf);
}

void obtain_time(void)
{
    initialize_sntp();

    // wait for time to be set
    time_t now = 0;
    struct tm timeinfo;
    int retry = 0;
    const int retry_count = 10;
    while(timeinfo.tm_year < (2016 - 1900) && ++retry < retry_count) {
        ESP_LOGI(SNTP_TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
    }

    // Set timezone to Eastern Standard Time and print local time
    char strftime_buf[64];
    setenv("TZ", "EST5EDT,M3.2.0/2,M11.1.0", 1);
    tzset();
    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    ESP_LOGI(SNTP_TAG, "The current date/time in New York is: %s", strftime_buf);
}

static void initialize_sntp(void)
{
    ESP_LOGI(SNTP_TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();
}