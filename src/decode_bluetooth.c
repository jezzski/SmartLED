
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"

#include "decode_bluetooth.h"

#include "scheduler.h"
#include "schedule_object.h"

//for set time code
#include "espsntp.h"

esp_err_t decode_ble_schedule(uint8_t* packet){
    schedule_object new_sch;
    if (packet[0] > NUM_CHANNELS)
    {
        uint32_t t = get_Int32(&packet[1]);
        set_time(t);
    }
    if(packet[1]==255){
        if(!packet[2]){//not rgb
            channel_on(packet[0], packet[3]);
        }
        else{
            set_color(packet[0], packet[4], packet[5], packet[6], packet[3]);
        }
    }else{
        new_sch.ID = packet[1];
        new_sch.name[0] = '\0';
        new_sch.enabled = 1;
        new_sch.start = get_Int32(&packet[2]);
        new_sch.duration = get_Int32(&packet[6]);
        new_sch.repeat_mask = packet[14];
        new_sch.repeat_time = get_Int32(&packet[15]);;
        new_sch.isRGB = 1;
        new_sch.brightness = packet[10];
        new_sch.r = packet[11];
        new_sch.g = packet[12];
        new_sch.b = packet[13];
        create_schedule(packet[0],new_sch);
    }
    return ESP_OK;
}

uint32_t get_Int32(uint8_t* begin){
    return (begin[0]) << 24 | (begin[1] << 16) | (begin [2] << 8) | begin[3];
}

