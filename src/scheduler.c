#include "scheduler.h"

//tag for logging
const static char *SCHEDULE_TAG = "SCHEDULER";
//task handle
static TaskHandle_t Schedule_Task = NULL;
//task init
static esp_err_t start_Schedule_Task(void);
//task code
static void Scheduler(void *pvParms);

//helper macros
#define MIN(a, b) a < b ? a : b
#define MAX(a, b) a > b ? a : b

void update_start_time(schedule_object *s, time_t curr)
{
    //math to update the start time based on repeat time/mask and current time
    s->enabled = 0; //just disable for now
}

void init_schedule(void)
{
    esp_err_t ret = start_Schedule_Task();
    if (ret != ESP_OK)
    {
        ESP_LOGE(SCHEDULE_TAG, "Unable to start task, ret=%s", esp_err_to_name(ret));
    }
    for (int i = 0; i < NUM_CHANNELS; ++i)
    {
        schedules[i] = NULL;
    }
    schedules[0] = malloc(sizeof(List));
    schedules[0]->next = NULL;
    schedules[1] = malloc(sizeof(List));
    schedules[1]->next = NULL;

    schedule_object s = {
        .ID = 0,
        .enabled = 1,
        .start = 20,
        .duration = 30,
        .repeat_mask = 0,
        .repeat_time = 60
    };
    strcpy(s.name, "Schedule1");
    schedules[0]->schedule = s;
    s.start = 10;
    s.duration = 60;
    strcpy(s.name, "Schedule2");
    schedules[1]->schedule = s;
}

//task init
static esp_err_t start_Schedule_Task(void)
{
    if (Schedule_Task != NULL)
    {
        ESP_LOGE(SCHEDULE_TAG, "Task already started!");
        return ESP_FAIL;
    }
    xTaskCreate(&Scheduler, "Scheduler", 2048, NULL, configMAX_PRIORITIES - 1, &Schedule_Task);
    return ESP_OK;
}

//task code
static void Scheduler(void *pvParms)
{
    printf("Task Started!\n");
    time_t curr;
    struct tm timeinfo = { 0 };
    char strftime_buf[64];

    uint32_t ulNotifiedValue;
    while(1)
    {
        time(&curr);
        localtime_r(&curr, &timeinfo);
        strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
        printf("Current time=%ld, %s\n", curr, strftime_buf);
        
        uint32_t nextTime = 0xFFFFFFFF; //next time for task to run
        for (int i = 0; i < NUM_CHANNELS; ++i) //check all schedules
        {
            List *it = schedules[i];
            while (it != NULL)
            {
                time(&curr); //get current time each loop?
                if (it->schedule.enabled)
                {
                    if (curr >= it->schedule.start) //check if ready
                    {
                        printf("Schedule Triggered!\n");
                        schedule_object t = it->schedule;
                        printf("[%d]%s Start:%d, Duration:%d, Mask:%d, Repeat:%d\n", t.ID, t.name, t.start, t.duration, t.repeat_mask, t.repeat_time);
                        //todo: dispatch update (color,brightness?)

                        //duration handling
                        if (it->schedule.duration != UINT32_MAX) //check if schedule has a duration
                        {
                            uint32_t finished = (it->schedule.start) + (it->schedule.duration);
                            if (curr >= finished) //duration done, update next times
                            {
                                //schedule finished
                                printf("Schedule duration done\n");
                                update_start_time(&(it->schedule), curr);
                            }
                            else
                            {
                                nextTime = MIN(nextTime, finished); //update next start time
                            }
                        }
                        else
                        {
                            //infinite duration, just update schedule
                            update_start_time(&(it->schedule), curr);
                        }

                    }
                    else
                    {
                        nextTime = MIN(nextTime, it->schedule.start);
                    }
                }
                it = it->next;
            }
        }
        
        
        printf("Next Scheduled Task Time:%ud\n\n", nextTime);
        
        TickType_t xTicksToWait = (((nextTime-curr)*1000)/portTICK_PERIOD_MS);
        if (nextTime==UINT32_MAX)
        {
            printf("Infinite\n");
            xTicksToWait = portMAX_DELAY;
        }
        
        xTaskNotifyWait( 0x00,      /* Don't clear any notification bits on entry. */
                         ULONG_MAX, /* Reset the notification value to 0 on exit. */
                         &ulNotifiedValue, /* Notified value pass out in
                                              ulNotifiedValue. */
                         xTicksToWait );  /* Block indefinitely. */
    }
    vTaskDelete(NULL);
}