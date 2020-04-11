#ifndef LED_H
#define LED_H
#ifdef _cplusplus
extern "C" {
#endif
    #include <stdio.h>
    #include "esp_system.h"
    #include "driver/ledc.h"
    
    #define GPIO_CHANNEL_0 32
    #define GPIO_CHANNEL_1 33
    #define GPIO_CHANNEL_2 25
    #define GPIO_CHANNEL_3 19
    #define GPIO_CHANNEL_4 18
    #define GPIO_CHANNEL_5 5
    
    #define NUM_CHANNELS 6
    
    #define LEDC_FADE_TIME 300 //max fade time in ms

    void init_channels(void);
    void channel_on(uint8_t index, uint8_t brightness);
    void channel_off(uint8_t index);
    void set_color(uint8_t index, uint16_t r, uint16_t g, uint16_t b, uint8_t brightness);
#ifdef _cplusplus
}
#endif
#endif