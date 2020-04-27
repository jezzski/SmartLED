/** \file
 * Description: Contains function declarations and settings to configure GPIO outputs to be used as PWM control for LED strips.
 * \author: Primary: Shipra Vaidya, Secondary: Jesse Cannon (shutdown features)
 * \date last modified: 4/26/2020
 */

#ifndef LED_H
#define LED_H
#ifdef _cplusplus
extern "C" {
#endif
    #include <stdio.h>
    #include "esp_system.h"
    #include "esp_log.h"
    #include "driver/ledc.h"
    
    /**
     * @brief Pin definitions for tech demo, needs to be changed for PCB.
     * 
     */
    #define GPIO_CHANNEL_0 32
    #define GPIO_CHANNEL_1 33
    #define GPIO_CHANNEL_2 25
    #define GPIO_CHANNEL_3 19
    #define GPIO_CHANNEL_4 18
    #define GPIO_CHANNEL_5 5
    
    /**
     * @brief Six pwm channels supported as either six individual channels, two RGB channels, or combination of two.
     * 
     */
    #define NUM_CHANNELS 6

    /**
     * @brief Shuts down all channels and prevents them from turning on again. Enter a permanent shutdown state until clear_shutdown is called or device is reset.
     * 
     * @return esp_err_t ESP_OK on success, ESP_FAIL if already in shutdown state.
     */
    esp_err_t shutdown_outputs(void);

    /**
     * @brief Clears a shutdown state, allowing outputs to be controlled again.
     * 
     * @return esp_err_t ESP_OK on success, ESP_FAIL if not in shutdown state.
     */
    esp_err_t clear_shutdown(void);

    /**
     * @brief Initialize channels to set all timer and channel configurations.
     * 
     */
    void init_channels(void);

    /**
     * @brief Sets brightness of individual channel to turn that output on.
     * 
     * @param index Value between 0 and 5 corresponding to channel being controlled.
     * @param brightness Value of brightness chosen per channel.
     */
    void channel_on(uint8_t index, uint8_t brightness);
    
    /**
     * @brief Sets brightness of individual channel to 0 to turn off channel output.
     * 
     * @param index Value between 0 and 5 corresponding to channel being controlled.
     */
    void channel_off(uint8_t index);
    
    /**
     * @brief Controls RGB channel by changing R, G, and B based off chosen brightness.
     * 
     * @param index Value between 0 and 1 corrensponding to two supported RGB channel outputs repectively.
     * @param r R value of chosen color.
     * @param g G value of chosen color.
     * @param b B value of chosen color.
     * @param brightness Value of brightness chosen for RGB channel.
     */
    void set_color(uint8_t index, uint16_t r, uint16_t g, uint16_t b, uint8_t brightness);
#ifdef _cplusplus
}
#endif
#endif