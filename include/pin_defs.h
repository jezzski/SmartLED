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
#define OC_ENABLE
#define OC_ALERT
#define OC_LATCH
//will be DAC
#define OC_LIMIT DAC1

#endif