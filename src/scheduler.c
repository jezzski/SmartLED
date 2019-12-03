#include "scheduler.h"

//todo: thread safe code

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

esp_err_t create_schedule(uint8_t channel, schedule_object s)
{
    printf("Channel: %x, Start: %d, Duration: %d, Repeat: %d", channel,s.start, s.duration, s.repeat_time);
    printf("Brightness: %x", s.brightness);
    printf("Red: %x, Green: %x, Blue: %x",s.r, s.g, s.b);
    //check if channel is valid
    if (channel > NUM_CHANNELS)
    {
        return ESP_ERR_INVALID_ARG;
    }
    //create new schedule node
    List *new = malloc(sizeof(List));
    if (new == NULL)
    {
        ESP_LOGE(SCHEDULE_TAG, "create_schedule unable to malloc new node!");
        return ESP_ERR_NO_MEM;
    }
    new->next = NULL;
    new->schedule = s;

    //append to list
    List *it = schedules[channel];
    if (it == NULL)
    {
        //at head of list
        schedules[channel] = new;
    }
    else
    {
        //find end of list
        while (it->next != NULL)
        {
            it = it->next;
        }
        it->next = new;
    }
    xTaskNotify(Schedule_Task, 0, eNoAction);
    return ESP_OK;
}

esp_err_t delete_schedule_by_id(uint8_t channel, uint8_t ID)
{
    if (channel > NUM_CHANNELS)
    {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_FAIL;
}

esp_err_t delete_schedule_by_name(uint8_t channel, char *name)
{
    if (channel > NUM_CHANNELS)
    {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_FAIL;
}

esp_err_t disable_schedule_by_id(uint8_t channel, uint8_t ID)
{
    if (channel > NUM_CHANNELS)
    {
        return ESP_ERR_INVALID_ARG;
    }

    List *it = schedules[channel];
    while (it != NULL)
    {
        if (it->schedule.ID == ID)
        {
            it->schedule.enabled = 0;
            return ESP_OK;
        }
        it = it->next;
    }
    return ESP_ERR_NOT_FOUND;
}

esp_err_t disable_schedule_by_name(uint8_t channel, char *name)
{
    if (channel > NUM_CHANNELS)
    {
        return ESP_ERR_INVALID_ARG;
    }
    List *it = schedules[channel];
    while (it != NULL)
    {
        if (strcmp(it->schedule.name, name))
        {
            it->schedule.enabled = 0;
            return ESP_OK;
        }
        it = it->next;
    }
    return ESP_ERR_NOT_FOUND;
}

esp_err_t enable_schedule_by_id(uint8_t channel, uint8_t ID)
{
    if (channel > NUM_CHANNELS)
    {
        return ESP_ERR_INVALID_ARG;
    }
    List *it = schedules[channel];
    while (it != NULL)
    {
        if (it->schedule.ID == ID)
        {
            it->schedule.enabled = 1;
            return ESP_OK;
        }
        it = it->next;
    }
    return ESP_ERR_NOT_FOUND;
}

esp_err_t enable_schedule_by_name(uint8_t channel, char *name)
{
    if (channel > NUM_CHANNELS)
    {
        return ESP_ERR_INVALID_ARG;
    }
    List *it = schedules[channel];
    while (it != NULL)
    {
        if (strcmp(it->schedule.name, name))
        {
            it->schedule.enabled = 1;
            return ESP_OK;
        }
        it = it->next;
    }
    return ESP_ERR_NOT_FOUND;
}

esp_err_t disable_all_schedules(void)
{
    for (int i = 0; i < NUM_CHANNELS; ++i)
    {
        List *it = schedules[i];
        while (it != NULL)
        {
            it->schedule.enabled = 0;
            it = it->next;
        }
    }
    return ESP_OK;
}

esp_err_t enable_all_schedules(void)
{
    for (int i = 0; i < NUM_CHANNELS; ++i)
    {
        List *it = schedules[i];
        while (it != NULL)
        {
            it->schedule.enabled = 1;
            it = it->next;
        }
    }
    return ESP_OK;
}

void update_start_time(schedule_object *s, time_t curr)
{
    //check if schedule is repeating
    if (!s->repeat_mask && !s->repeat_time)
    {
        s->enabled = 0;
        return;
    }
    //todo: revisit this logic
    //does this support lights on twice a day, once a day?
    //should repeat_time stop after day ends or carry on into next day?

    //if schedule runs every x seconds
    if (s->repeat_time)
    {
        s->start += s->repeat_time;
        return;
    }
    //schedule runs on certain days
    struct tm time_info = { 0 };
    localtime_r(&curr, &time_info);
    //find difference between current day and next run day
    uint8_t currDay = time_info.tm_wday; //current day of week
    uint8_t nextDay = 1; //days until next run, at least 1
    for (int i = currDay + 1; i != currDay; ++i)
    {
        if (i > 6) i = 0;
        //check if schedule runs this day
        if ((s->repeat_mask >> (6-i)) & 1)
        {
            s->start += (86400*nextDay);
            break;
        }
        ++nextDay;
    }
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
        
        uint32_t nextTime = UINT32_MAX; //next time for task to run, default to infinite
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
                        if (t.isRGB)
                        {
                            set_color(i, t.r, t.g, t.b, t.brightness);
                        }
                        else
                        {
                            channel_on(i, t.brightness);
                        }

                        //duration handling
                        if (it->schedule.duration != UINT32_MAX) //check if schedule has a duration
                        {
                            uint32_t finished = (it->schedule.start) + (it->schedule.duration);
                            if (curr >= finished) //duration done, update next times
                            {
                                //schedule finished
                                printf("Schedule duration done\n");
                                //todo: turn off RGB with same function
                                if (!it->schedule.isRGB)
                                    channel_off(i);
                                else
                                    set_color(i, 0, 0, 0, 0);
                                update_start_time(&(it->schedule), curr);
                                nextTime = MIN(nextTime, it->schedule.start);
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
                            nextTime = MIN(nextTime, it->schedule.start);
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
        
        
        printf("Next Scheduled Task Time:%u\n\n", nextTime);
        
        TickType_t xTicksToWait = (((nextTime-curr)*1000)/portTICK_PERIOD_MS);
        if (nextTime==UINT32_MAX)
        {
            printf("Infinite\n");
            xTicksToWait = portMAX_DELAY;
        }
        printf("curr:%ld, Next:%u\n", curr, nextTime);
        printf("Diff(s)%ld, Ticks:%d, Period:%d\n", (nextTime-curr), xTicksToWait, portTICK_PERIOD_MS);
        xTaskNotifyWait( 0x00,      /* Don't clear any notification bits on entry. */
                         ULONG_MAX, /* Reset the notification value to 0 on exit. */
                         &ulNotifiedValue, /* Notified value pass out in
                                              ulNotifiedValue. */
                         xTicksToWait );  /* Block indefinitely. */
    }
    vTaskDelete(NULL);
}