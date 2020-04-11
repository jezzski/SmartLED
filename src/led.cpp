#include "led.h"

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
    brightness <<= 4;
    switch(index)
    {
        case 0:
            ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, brightness, LEDC_FADE_TIME);
            ledc_fade_start(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT);
            break;
        case 1:
            ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, brightness, LEDC_FADE_TIME);
            ledc_fade_start(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, LEDC_FADE_NO_WAIT);
            break;
        case 2:
            ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, brightness, LEDC_FADE_TIME);
            ledc_fade_start(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, LEDC_FADE_NO_WAIT);
            break;
        case 3:
            ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, brightness, LEDC_FADE_TIME);
            ledc_fade_start(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, LEDC_FADE_NO_WAIT);
            break;            
        case 4:
            ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_4, brightness, LEDC_FADE_TIME);
            ledc_fade_start(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_4, LEDC_FADE_NO_WAIT);
            break;
        case 5:
            ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_5, brightness, LEDC_FADE_TIME);
            ledc_fade_start(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_5, LEDC_FADE_NO_WAIT);
            break;
        default: break;
    }    
}

void channel_off(uint8_t index)
{
    switch(index)
    {
        case 0:
            ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0, LEDC_FADE_TIME);
            ledc_fade_start(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT);
            break;
        case 1:
            ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 0, LEDC_FADE_TIME);
            ledc_fade_start(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, LEDC_FADE_NO_WAIT);
            break;
        case 2:
            ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, 0, LEDC_FADE_TIME);
            ledc_fade_start(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, LEDC_FADE_NO_WAIT);
            break;
        case 3:
            ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, 0, LEDC_FADE_TIME);
            ledc_fade_start(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, LEDC_FADE_NO_WAIT);
            break;            
        case 4:
            ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_4, 0, LEDC_FADE_TIME);
            ledc_fade_start(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_4, LEDC_FADE_NO_WAIT);
            break;
        case 5:
            ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_5, 0, LEDC_FADE_TIME);
            ledc_fade_start(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_5, LEDC_FADE_NO_WAIT);
            break;
        default: break;
    }
}

void set_color(uint8_t index, uint16_t r, uint16_t g, uint16_t b, uint8_t brightness)
{
    r *= ((float)brightness / UINT8_MAX);
    g *= ((float)brightness / UINT8_MAX);
    b *= ((float)brightness / UINT8_MAX);

    switch(index)
    {
        case 0:
            ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, r << 4, LEDC_FADE_TIME);
            ledc_fade_start(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT);
            
            ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, g << 4, LEDC_FADE_TIME);
            ledc_fade_start(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, LEDC_FADE_NO_WAIT);
            
            ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, b << 4, LEDC_FADE_TIME);
            ledc_fade_start(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, LEDC_FADE_NO_WAIT);
            
        break;
        case 1:
            ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, r << 4, LEDC_FADE_TIME);
            ledc_fade_start(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, LEDC_FADE_NO_WAIT);
            
            ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_4, g << 4, LEDC_FADE_TIME);
            ledc_fade_start(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_4, LEDC_FADE_NO_WAIT);
            
            ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_5, b << 4, LEDC_FADE_TIME);
            ledc_fade_start(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_5, LEDC_FADE_NO_WAIT);
        break;
        default: break;
    }
}
