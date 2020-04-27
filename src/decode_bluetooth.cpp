/** \file
 * Description: 
 * \author: Hunaid Puri
 * \date last modified: 4/26/2020
 */


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"

#include "decode_bluetooth.h"

#include "scheduler.h"
#include "schedule_object.h"

//for set time code
#include "espsntp.h"
#include "esp_gatts_api.h"

uint8_t schedule_value[20];
uint8_t saved_channel_num;
char schedule_name[20];
extern List *schedules[NUM_CHANNELS];
List* it;
char saved_name[21];
uint16_t saved_len;
uint8_t state=4;
uint8_t done_string[10]="Done Read";

esp_err_t start_schedule_read(uint8_t* packet, uint16_t handle){
    if(!strcmp("Begin Read",(const char*)(packet+1))){
        saved_channel_num=packet[0]-48;
        state=0;
        set_schedule_read(handle);
    }
    return ESP_OK;
}
esp_err_t set_schedule_read(uint16_t handle){
    switch(state){
        case 0:{
            it = schedules[saved_channel_num];
            if(it!=NULL){
                state++;
            }
            else{
                state=3;
                break;
            }
        }
        case 1:{
            memset(schedule_name,0,20);
            strcpy(schedule_name,it->schedule.name);
            esp_ble_gatts_set_attr_value(handle,strlen(schedule_name)*sizeof(uint8_t),(uint8_t*)schedule_name);
            state++;
            break;
        }
        case 2:{
            schedule_value[0]=0;
            schedule_value[1]=it->schedule.ID;
            //it->schedule.name[0] = '\0';
            //it->schedule.enabled = 1;
            schedule_value[2]=(it->schedule.start>>24);
            schedule_value[3]=((it->schedule.start&0x00FF0000)>>16);
            schedule_value[4]=((it->schedule.start&0x0000FF00)>>8);
            schedule_value[5]=(it->schedule.start&0x000000FF);
            schedule_value[6]=(it->schedule.duration>>24);
            schedule_value[7]=((it->schedule.duration&0x00FF0000)>>16);
            schedule_value[8]=((it->schedule.duration&0x0000FF00)>>8);
            schedule_value[9]=(it->schedule.duration&0x000000FF);
            schedule_value[14]=it->schedule.repeat_mask;
            schedule_value[15]=(it->schedule.repeat_time>>24);
            schedule_value[16]=((it->schedule.repeat_time&0x00FF0000)>>16);
            schedule_value[17]=((it->schedule.repeat_time&0x0000FF00)>>8);
            schedule_value[18]=(it->schedule.repeat_time&0x000000FF);
            //schedule_value[1]=it->schedule.isRGB = 1;
            schedule_value[10]=it->schedule.brightness;
            schedule_value[11]=it->schedule.r;
            schedule_value[12]=it->schedule.g;
            schedule_value[13]=it->schedule.b;
            esp_ble_gatts_set_attr_value(handle,sizeof(schedule_value),schedule_value);
            it=it->next;
            if(it==NULL){
                state=3;
            }
            else{
                state=1;
            }
            break;
        }
        case 3:{
            esp_ble_gatts_set_attr_value(handle,sizeof(done_string),done_string);
            state=4;
            break;
        }
        default:
            break;
    }
    return ESP_OK;
}

esp_err_t decode_ble_time(uint8_t* packet){
    if (packet[0] > NUM_CHANNELS)
    {
        uint32_t t = get_Int32(&packet[1]);
        set_time(t);
    }
    return ESP_OK;
}
esp_err_t decode_ble_direct(uint8_t* packet){
     if(packet[1]==255){
        if(!packet[2]){//not rgb
            channel_on(packet[0], packet[3]);
        }
        else{
            set_color(packet[0], packet[4], packet[5], packet[6], packet[3]);
        }
    }
    return ESP_OK;
}
esp_err_t decode_ble_delete(uint8_t* packet){
    return ESP_OK;
}
esp_err_t decode_ble_schedule_name(uint8_t* packet, uint16_t length){
    memset(saved_name,0,21);
    strncpy(saved_name,(const char*)packet,length);
    saved_len=length;
    return ESP_OK;
}
esp_err_t decode_ble_schedule(uint8_t* packet){
    schedule_object new_sch;
   if(packet[0] <= NUM_CHANNELS && packet[1]!=255){
        new_sch.ID = packet[1];
        strcpy(new_sch.name,saved_name);
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

