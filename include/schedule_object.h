/** \file
 * Description: Contains the definition of the data object used for schedules in the scheduler subsystem. Defines a simple linked list data structure to contain schedule data object.
 * \author: Jesse Cannon
 * \date last modified: 4/26/2020
 */

#ifndef SCHEDULE_OBJECT_H
#define SCHEDULE_OBJECT_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

//todo: revisit with C++ style code? would make certain actions way easier
/**
 * @brief data object that specified parameters a schedule can contain
 * 
 */
typedef struct Schedule_Object
{
    uint8_t ID; /**< The ID for the schedule. Each schedule in a list should have its own unique ID. */
    char name[250]; /**< The name of the schedule. Each schedule in a list should have its own unique name. */
    uint8_t enabled; /**< The enabled status of the schedule. If 0, the schedule does not run. If not 0 the schedule actively runs according to other parameters. */
    uint32_t start; /**< The unix timestamp of the start time. If a schedule repeats, this field will update to the next start time as the schedule runs. */
    uint32_t duration; /**< The duration, in seconds, of the schedule. If set to the max value the schedule will run indefinitely. */
    uint8_t repeat_mask; /**< A bit mask for which days the schedule runs on. If repeat_time is set this field is not used. The MSB bit is not used and following bits represent the days. The 8 bits represent [Unused, Sunday, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday]. */
    uint32_t repeat_time; /**< An alternative way to have schedules repeat instead of by day. This field represents the time, in seconds, that the schedule repeats. The next start time is calculated by adding the current start time plus the repeat_time. Settings this field to 5 seconds means that the schedule's start is triggered every 5 seconds. If this field is enabled then repeat_mask and dawn/dusk will not work. */
    uint8_t dawn; /**< Whether the schedule runs at dawn. Not compatable with repeat_time */
    uint8_t dusk;/**< Whether the schedule runs at dusk. Not compatable with repeat_time. */

    //actions to take
    uint8_t isRGB; /**< The type of LED associated. If this field is 0, the LED is not an RGB LED and only the brightness field is used in controlling the LED. If isRGB is not 0, the brightness, R, G, and B fields control the LED */
    uint8_t brightness; /**< The brightness of the LED. 0-255 with 0 being off, and 255 being maximum brightness. */
    uint8_t r; /**< The red portion of the color. Values from 0-255. */
    uint8_t g; /**< The green portion of the color. Values from 0-255. */
    uint8_t b; /**< The blue portion of the color. Values from 0-255. */
} schedule_object;

/**
 * @brief Nodes of a singly linked list data structure.
 * 
 */
typedef struct Node
{
    schedule_object schedule; /**< the schedule associated with this node */
    struct Node *next; /**< Pointer to the next node in the linked list data structure */
} List;
#endif