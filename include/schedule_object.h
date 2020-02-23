#ifndef SCHEDULE_OBJECT_H
#define SCHEDULE_OBJECT_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

//object for schedules
//todo: revisit with C++ style code? would make certain actions way easier
typedef struct Schedule_Object
{
    uint8_t ID;
    char name[250];
    uint8_t enabled;
    uint32_t start;
    uint32_t duration;
    uint8_t repeat_mask; //Not used, Su, M, T, W, Th, F, Sa
    uint32_t repeat_time; //for repeating through the day, can't fit 24 hours in 16 bits

    //actions to take
    uint8_t isRGB; //could combine with a previous field to save memory space
    uint8_t brightness;
    uint8_t r;
    uint8_t g;
    uint8_t b;
} schedule_object;

//list of schedules
typedef struct Node
{
    schedule_object schedule;
    struct Node *next;
} List;
#endif