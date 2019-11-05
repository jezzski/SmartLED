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

    // Open
    printf("\n");
    printf("Opening Non-Volatile Storage (NVS) handle... ");
    nvs_handle my_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        printf("Done\n");

        // Read
        printf("Reading restart counter from NVS ... ");
        int32_t restart_counter = 0; // value will default to 0, if not set yet in NVS
        err = nvs_get_i32(my_handle, "restart_counter", &restart_counter);
        switch (err) {
            case ESP_OK:
                printf("Done\n");
                printf("Restart counter = %d\n", restart_counter);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                printf("The value is not initialized yet!\n");
                break;
            default :
                printf("Error (%s) reading!\n", esp_err_to_name(err));
        }

        // Write
        printf("Updating restart counter in NVS ... ");
        restart_counter++;
        err = nvs_set_i32(my_handle, "restart_counter", restart_counter);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        // Commit written value.
        // After setting any values, nvs_commit() must be called to ensure changes are written
        // to flash storage. Implementations may write to storage at other times,
        // but this is not guaranteed.
        printf("Committing updates in NVS ... ");
        err = nvs_commit(my_handle);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        // Close
        nvs_close(my_handle);



        //quick pwm test
        ledc_timer_config_t ledc_timer_test = {
            .duty_resolution = LEDC_TIMER_12_BIT,
            .freq_hz = 5000,
            .speed_mode = LEDC_HIGH_SPEED_MODE,
            .timer_num = LEDC_TIMER_0
        };

        ledc_timer_config(&ledc_timer_test);

        int dutyVal = 0;
        int test = restart_counter % 3;
        switch (test)
        {
            case 0:
                dutyVal = 0xF00;
            break;
            case 1:
                dutyVal = 0x100;
            break;
            case 2:
                dutyVal = 0;
            break;
            default:break;
        }
        printf("%x\n", dutyVal);
        ledc_channel_config_t ledc_channel_test = {
            .channel = LEDC_CHANNEL_0,
            .duty = dutyVal,
            .gpio_num = 23,
            .speed_mode = LEDC_HIGH_SPEED_MODE,
            .hpoint = 0,
            .timer_sel = LEDC_TIMER_0};
        ledc_channel_config(&ledc_channel_test);
    }


    ledc_channel_config_t ledc_channel_red = {
        .channel = LEDC_CHANNEL_1,
        .duty = 0,
        .gpio_num = 21,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER_0};
    ledc_channel_config(&ledc_channel_red);

    ledc_channel_config_t ledc_channel_green = {
        .channel = LEDC_CHANNEL_2,
        .duty = 0,
        .gpio_num = 19,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER_0};
    ledc_channel_config(&ledc_channel_green);

    ledc_channel_config_t ledc_channel_blue = {
        .channel = LEDC_CHANNEL_3,
        .duty = 0,
        .gpio_num = 22,
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
        //printf("Duty:%x\n", duty);
        vTaskDelay(/*50*/1000 / portTICK_PERIOD_MS);
    }
}