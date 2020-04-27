/** \file
 * Description: Pin definitions for external hardware. Used by measurement subsystem for INA300 for overcurrent detection
 * \author: Jesse Cannon
 * \date last modified: 4/26/2020
 */

#ifndef PIN_DEFS_H
#define PIN_DEFS_H

//todo: These will need to change: there is an errata with GPIO 36 and 39 when certain peripherals are active (including some used for Wi-Fi)
#define CH3_HIGH 39
#define CH3_LOW 36

//can use either for alpha PCB
//#define DAC1 25
#define DAC1 1
#define DAC2 26

//INA 300 Pins for OC
//TODO: must modify these to use updated pins from PCB
#define OC_ENABLE GPIO_NUM_22
#define OC_ALERT GPIO_NUM_26
#define OC_LATCH
//will be DAC
#define OC_LIMIT DAC1

#endif