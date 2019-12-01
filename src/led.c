#include "led.h"

struct channel
{
    char name[250];
    uint16_t r;
    uint16_t g;
    uint16_t b;
    uint8_t brightness;
};

void init_channels(void)
{
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_12_BIT,
        .freq_hz = 5000,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_0
    };

    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel_0 = {
        .channel = LEDC_CHANNEL_0,
        .duty = 0,
        .gpio_num = GPIO_CHANNEL_0,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER_0
    };

    ledc_channel_config(&ledc_channel_0);

    ledc_channel_config_t ledc_channel_1 = {
        .channel = LEDC_CHANNEL_1,
        .duty = 0,
        .gpio_num = GPIO_CHANNEL_1,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER_0,
    };

    ledc_channel_config(&ledc_channel_1);

    ledc_channel_config_t ledc_channel_2 = {
        .channel = LEDC_CHANNEL_2,
        .duty = 0,
        .gpio_num = GPIO_CHANNEL_2,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER_0
    };

    ledc_channel_config(&ledc_channel_2);

    ledc_channel_config_t ledc_channel_3 = {
        .channel = LEDC_CHANNEL_3,
        .duty = 0,
        .gpio_num = GPIO_CHANNEL_3,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER_0,
    };

    ledc_channel_config(&ledc_channel_3);

    ledc_channel_config_t ledc_channel_4 = {
        .channel = LEDC_CHANNEL_4,
        .duty = 0,
        .gpio_num = GPIO_CHANNEL_4,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER_0,
    };

    ledc_channel_config(&ledc_channel_4);

    ledc_channel_config_t ledc_channel_5 = {
        .channel = LEDC_CHANNEL_5,
        .duty = 0,
        .gpio_num = GPIO_CHANNEL_5,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER_0,
    };

    ledc_channel_config(&ledc_channel_5);
}

void channel_on(uint8_t index, uint8_t brightness)
{
    switch(index)
    {
        case 0:
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, brightness);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
            break;
        case 1:
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, brightness);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
            break;
        case 2:
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, brightness);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2);
            break;
        case 3:
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, brightness);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3);
            break;            
        case 4:
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_4, brightness);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_4);
            break;
        case 5:
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_5, brightness);
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
    brightness = (float) brightness / UINT8_MAX;
    r *= brightness;
    g *= brightness;
    b *= brightness;

    switch(index)
    {
        case 0:
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, r);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
            
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, g);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
            
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, b);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2);
            
        break;
        case 1:
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, r);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3);
            
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_4, g);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_4);
            
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_5, b);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_5);
        break;
        default: break;
    }
}
