/** \file
 * Description: Contains function and pin definitions for the time manager subsystem. Controls external RTC MCP79510.
 * see: http://ww1.microchip.com/downloads/en/DeviceDoc/MCP7951X-MCP7952X-Battery-Backed-SPI-RTCC-20002300C.pdf
 * \author: Lead: Shipra Vaidya, Seoncdary: Jesse Cannon
 * \date last modified: 4/26/2020
 */

#ifndef RTC_DEFINE_H
#define RTC_DEFINE_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "esp_log.h"
#include <time.h>
#include <sys/time.h>
#include "driver/spi_master.h"
#include "esp_log.h"
#include "esp_spi_flash.h"
#include "sdkconfig.h"

#define HSPI_MISO 12
#define HSPI_MOSI 13
#define HSPI_CLK 14
#define HSPI_CS 15

//instructions
#define EEREAD 0x03
#define EEWRITE 0x02
#define EEWRDI 0x04
#define EEWREN 0x06
#define SRREAD 0x05
#define SSWRITE 0x01

//commands
#define RTC_READ 0x13
#define RTC_WRITE 0x12

#define RTC_UNLOCK 0x14
#define IDWRITE 0x32
#define IDREAD 0x33
#define CLRRAM 0x54

//time addresses
#define  RTCHSEC 0x00 	//hundredth of SECONDS
#define  RTCSEC	0x01	//SECONDS
#define  RTCMIN	0x02	//MINUTES
#define  RTCHOUR 0x03	//HOURS
#define  RTCWKDAY 0x04	//DAY OF WK
#define  RTCDATE 0x05	//DATE
#define  RTCMTH	0x06	//MONTH
#define  RTCYEAR 0x07	//YEAR
#define  CONTROL 0x08	//CONTROL
#define  OSCTRIM 0x09	//OSC TRIMMING

//power outage addresses
#define  PWRDNMIN 0x18	//Minutes
#define  PWRDNHOUR 0x19	//Hour
#define  PWRDNDATE 0x1A	//DATE
#define  PWRDNMONTH	0x1B	//MONTH

//global RTC constants/bitmasks
#define  ST	0x80	//START OSC					(RTCSEC REG)
#define  LPYR 0x20	//MASK FOR LEAP YEAR		(RTCMTH REG)
#define  PM	0x20	//post-meridian bit 		(RTCHOUR REG)
#define  H12 0x40	//12 HOURS FORMAT			(RTCHOUR REG)
#define  TRIMSIGN 0x80	//minus sign for the calibration reg
#define  OUT 0x80	//= SET CLKO PIN			(CONTROL REG)
#define  SQWEN 0x40	//enable SQWAVE ON CLKOUT	(CONTROL REG)

#define  OSCRUN	0x20	//state of the oscillator	(running/not)
#define  VBATEN 0x08	//enable battery BACKUP		(RTCWKDAY REG)
#define  PWRFAIL 0x10	//Flag for power failure	(RTCWKDAY REG)

#define RTC_TAG "RTC"



/**
 * @brief Set the time of the external RTC to the specified value.
 * 
 * @param time The time to be set on the device.
 * @return esp_err_t ESP_OK on success, else on failure.
 */
esp_err_t setTime(const struct tm *time);

/**
 * @brief Get the current time on the external RTC.
 * 
 * @param outTime The current time on the RTC will be stored in this value. Check the return type against ESP_OK to ensure this value was set correctly.
 * @return esp_err_t ESP_OK on success, else on failure.
 */
esp_err_t getTime(struct tm *outTime);

/**
 * @brief The RTOS task that handles RTC related actions. This task syncrhonizes the esp32's and RTC's time.
 * 
 * @param pvParms Required parameter of RTOS tasks. Not used in this task.
 */
void RTCHandler(void *pvParms);

/**
 * @brief Starts the RTOS task that handles RTC and esp32 time synchronization.
 * 
 * @return esp_err_t ESP_OK if task started successfully, else on failure.
 */
esp_err_t ST_StartRTCHandler(void);

/**
 * @brief The task handle for the RTC task. If this field is != NULL then task is currently running. Can be used to stop task with RTOS functions.
 * 
 */
TaskHandle_t RTC_Handle;

/**
 * @brief A copy of the most recent time obtained from the RTC. Is not guaranteed to be accurate to the actual time stored on the RTC. This value is updated as often as the RTC task runs.
 * 
 */
struct tm *currTime;


#endif