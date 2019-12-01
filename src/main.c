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

void app_main()
{
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );

    //init wifi
    //wifi_init_sta();

    //obtain_time();

    //init_schedule();

    //quick pwm test
    ledc_timer_config_t ledc_timer_test = {
        .duty_resolution = LEDC_TIMER_12_BIT,
        .freq_hz = 5000,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_0
    };

    ledc_timer_config(&ledc_timer_test);

    ledc_channel_config_t ledc_channel_test = {
        .channel = LEDC_CHANNEL_0,
        .duty = 0xFFF,
        .gpio_num = 25,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER_0};
    ledc_channel_config(&ledc_channel_test);


    ledc_channel_config_t ledc_channel_red = {
        .channel = LEDC_CHANNEL_1,
        .duty = 0,
        .gpio_num = 19,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER_0};
    ledc_channel_config(&ledc_channel_red);

    ledc_channel_config_t ledc_channel_green = {
        .channel = LEDC_CHANNEL_2,
        .duty = 0,
        .gpio_num = 18,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER_0};
    ledc_channel_config(&ledc_channel_green);

    ledc_channel_config_t ledc_channel_blue = {
        .channel = LEDC_CHANNEL_3,
        .duty = 0,
        .gpio_num = 5,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER_0};
    ledc_channel_config(&ledc_channel_blue);

    uint32_t duty = ledc_get_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);

    bool up = true;
    uint16_t r = 0;
    uint16_t g = 0;
    uint16_t b = 0;
    while (1)
    {
        uint32_t color = esp_random(); //RRGGBB
        r = (color >> 16) & 0xFF;
        g = (color >> 12) & 0xFF;
        b = color & 0xFF;

        r <<= 4;
        g <<= 4;
        b <<= 4;
        printf("R:%x G:%x B:%x\n", r, g, b);


        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, r);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);

        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, g);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2);

        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, b);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3);

        if (up)
        {
            duty += 100;
            if (duty > 0xFFF)
            {
                duty = 0xFFF;
                up = false;
            }
        }
        else
        {
            duty -= 100;
            if (duty <= 0 || duty > 0xFFFF)
            {
                duty = 0;
                up = true;
            }
        }
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, duty);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
        printf("Duty:%x\n", duty);
        vTaskDelay(/*50*/1000 / portTICK_PERIOD_MS);
    }

    /*while (1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }*/
}