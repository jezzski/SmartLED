#ifndef SCHEDULE_OBJECT_H
#define SCHEDULE_OBJECT_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

typedef struct Schedule_Object
{
    uint8_t ID;
    char name[250];
    uint8_t enabled;
    uint32_t start;
    uint32_t duration;
    uint8_t repeat_mask; //EN, S, M, T, W, Th, F, S
    uint32_t repeat_time; //for repeating through the day, can't fit 24 hours in 16 bits
} schedule_object;

typedef struct Node
{
    schedule_object schedule;
    struct Node *next;
} List;

#endif