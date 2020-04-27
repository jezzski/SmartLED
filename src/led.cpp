/** \file
 * Description: Contains function definitions to setup and manipulate GPIO outputs as PWM outputs for controlling LED strips
 * \author: Primary: Shipra Vaidya, Secondary: Jesse Cannon (bugfixes and shutdown feature)
 * \date last modified: 4/26/2020
 */

#include "led.h"

struct channel
{
    char name[250];
    uint16_t r;
    uint16_t g;
    uint16_t b;
    uint8_t brightness;
};

uint8_t shutdown_status = 0;

esp_err_t shutdown_outputs(void)
{
    for (int i = 0; i < NUM_CHANNELS; i++)
    {
        channel_off(i);
    }

    if (shutdown_status)
    {
        ESP_LOGI("LED", "Already in shutdown state!");
        return ESP_FAIL;
    }
    else
    {
        shutdown_status = 1;
        return ESP_OK;
    }
}

esp_err_t clear_shutdown(void)
{
    if (!shutdown_status)
    {
        ESP_LOGI("LED", "Not in shutdown state!");
        return ESP_FAIL;
    }
    else
    {
        shutdown_status = 0;
        return ESP_OK;
    }
}

void init_channels(void)
{
    ledc_timer_config_t ledc_timer;
    ledc_timer.duty_resolution = LEDC_TIMER_12_BIT;
    ledc_timer.freq_hz = 5000;
    ledc_timer.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_timer.timer_num = LEDC_TIMER_0;

    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel_0;
    ledc_channel_0.channel = LEDC_CHANNEL_0;
    ledc_channel_0.duty = 0;
    ledc_channel_0.gpio_num = GPIO_CHANNEL_0;
    ledc_channel_0.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel_0.hpoint = 0;
    ledc_channel_0.timer_sel = LEDC_TIMER_0;

    ledc_channel_config(&ledc_channel_0);

    ledc_channel_config_t ledc_channel_1;
    ledc_channel_1.channel = LEDC_CHANNEL_1;
    ledc_channel_1.duty = 0;
    ledc_channel_1.gpio_num = GPIO_CHANNEL_1;
    ledc_channel_1.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel_1.hpoint = 0;
    ledc_channel_1.timer_sel = LEDC_TIMER_0;

    ledc_channel_config(&ledc_channel_1);

    ledc_channel_config_t ledc_channel_2;
    ledc_channel_2.channel = LEDC_CHANNEL_2;
    ledc_channel_2.duty = 0;
    ledc_channel_2.gpio_num = GPIO_CHANNEL_2;
    ledc_channel_2.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel_2.hpoint = 0;
    ledc_channel_2.timer_sel = LEDC_TIMER_0;

    ledc_channel_config(&ledc_channel_2);

    ledc_channel_config_t ledc_channel_3;
    ledc_channel_3.channel = LEDC_CHANNEL_3;
    ledc_channel_3.duty = 0;
    ledc_channel_3.gpio_num = GPIO_CHANNEL_3;
    ledc_channel_3.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel_3.hpoint = 0;
    ledc_channel_3.timer_sel = LEDC_TIMER_0;

    ledc_channel_config(&ledc_channel_3);

    ledc_channel_config_t ledc_channel_4;
    ledc_channel_4.channel = LEDC_CHANNEL_4;
    ledc_channel_4.duty = 0;
    ledc_channel_4.gpio_num = GPIO_CHANNEL_4;
    ledc_channel_4.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel_4.hpoint = 0;
    ledc_channel_4.timer_sel = LEDC_TIMER_0;

    ledc_channel_config(&ledc_channel_4);

    ledc_channel_config_t ledc_channel_5;
    ledc_channel_5.channel = LEDC_CHANNEL_5;
    ledc_channel_5.duty = 0;
    ledc_channel_5.gpio_num = GPIO_CHANNEL_5;
    ledc_channel_5.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel_5.hpoint = 0;
    ledc_channel_5.timer_sel = LEDC_TIMER_0;

    ledc_channel_config(&ledc_channel_5);
}

void channel_on(uint8_t index, uint8_t brightness)
{
    if (shutdown_status)
    {
        ESP_LOGW("LED", "Cannot control LED's while in shutdown status!");
        return;
    }

    int br = brightness << 4;
    printf("Brightness:%d\n", br);
    switch(index)
    {
        case 0:
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, br);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
            break;
        case 1:
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, br);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
            break;
        case 2:
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, br);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2);
            break;
        case 3:
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, br);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3);
            break;            
        case 4:
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_4, br);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_4);
            break;
        case 5:
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_5, br);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_5);
            break;
        default: break;
    }    
}

void channel_off(uint8_t index)
{
    switch(index)
    {
        case 0:
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
            break;
        case 1:
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 0);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
            break;
        case 2:
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, 0);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2);
            break;
        case 3:
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, 0);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3);
            break;            
        case 4:
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_4, 0);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_4);
            break;
        case 5:
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_5, 0);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_5);
            break;
        default: break;
    }
}

void set_color(uint8_t index, uint16_t r, uint16_t g, uint16_t b, uint8_t brightness)
{
    if (shutdown_status)
    {
        ESP_LOGW("LED", "Cannot control LED's while in shutdown status!");
        return;
    }
    r *= ((float)brightness / UINT8_MAX);
    g *= ((float)brightness / UINT8_MAX);
    b *= ((float)brightness / UINT8_MAX);

    switch(index)
    {
        case 0:
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, r << 4);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
            
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, g << 4);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
            
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, b << 4);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2);
            
        break;
        case 1:
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, r << 4);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3);
            
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_4, g << 4);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_4);
            
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_5, b << 4);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_5);
        break;
        default: break;
    }
}