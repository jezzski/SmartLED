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
#define  ST	0x80
#define  LPYR 0x20
#define  PM	0x20
#define  H12 0x40
#define  TRIMSIGN
#define  OUT
#define  SQWEN

#define  OSCRUN
#define  VBATEN
#define  PWRFAIL

#define RTC_TAG "RTC"



//functions
esp_err_t setTime(const struct tm *time);
esp_err_t getTime(struct tm *outTime);

void RTCHandler(void *pvParms);
esp_err_t ST_StartRTCHandler(void);
TaskHandle_t RTC_Handle;

struct tm *currTime;


#endif