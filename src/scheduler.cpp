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

//schedule list for each channel
List *schedules[NUM_CHANNELS];

//helper macros
#define MIN(a, b) a < b ? a : b
#define MAX(a, b) a > b ? a : b

esp_err_t create_schedule(uint8_t channel, schedule_object s)
{
    //here
    if (s.duration == 12345)
    {
        //delete schedules
        delete_all_schedules();
        return ESP_OK;
    }
    //end here

    printf("Channel: %x, Start: %d, Duration: %d, Repeat: %d", channel,s.start, s.duration, s.repeat_time);
    printf("Brightness: %x", s.brightness);
    printf("Red: %x, Green: %x, Blue: %x",s.r, s.g, s.b);
    printf("ID:%d, Name:%s\n", s.ID, s.name);
    //check if channel is valid
    if (channel >= NUM_CHANNELS)
    {
        return ESP_ERR_INVALID_ARG;
    }
    //create new schedule node
    List *newSched = (List*)malloc(sizeof(List));
    if (newSched == NULL)
    {
        ESP_LOGE(SCHEDULE_TAG, "create_schedule unable to malloc new node!");
        return ESP_ERR_NO_MEM;
    }
    newSched->next = NULL;
    newSched->schedule = s;

    //append to list
    List *it = schedules[channel];
    if (it == NULL)
    {
        //make head of list
        schedules[channel] = newSched;
    }
    else
    {
        //if the schedule already exists, we simply replace it
        if ((strcmp(it->schedule.name, newSched->schedule.name) == 0) || (it->schedule.ID == newSched->schedule.ID))
        {
            it->schedule = newSched->schedule;
            free(newSched); //free used memory since node already existed
            //notify scheduler of an update so it checks if any schedules should change status
            xTaskNotify(Schedule_Task, 0, eNoAction);
            return ESP_OK;
        }
        //find end of list
        while (it->next != NULL)
        {
            it = it->next;
        }
        //append
        it->next = newSched;
    }
    //notify scheduler of an update so it checks if any schedules should change status
    xTaskNotify(Schedule_Task, 0, eNoAction);
    return ESP_OK;
}

esp_err_t delete_schedule_by_id(uint8_t channel, uint8_t ID)
{
    if (channel >= NUM_CHANNELS)
    {
        return ESP_ERR_INVALID_ARG;
    }
    List *it = schedules[channel];
    List *prev = NULL;
    //iterate through channel and try to find schedule to delete
    while (it != NULL)
    {
        if (it->schedule.ID == ID)
        {
            //schedule found, delete and update next pointers
            if (prev == NULL)
            {
                //the head node needs to be deleted, free and set head to NULL
                free(it);
                schedules[channel] = NULL;
                xTaskNotify(Schedule_Task, 0, eNoAction);
                return ESP_OK;
            }
            else
            {
                //isn't head, update next pointers and free memory
                prev->next = it->next;
                free(it);
                xTaskNotify(Schedule_Task, 0, eNoAction);
                return ESP_OK;
            }
        }
        prev = it;
        it = it->next;
    }
    return ESP_ERR_NOT_FOUND;
}

esp_err_t delete_schedule_by_name(uint8_t channel, char *name)
{
    if (channel >= NUM_CHANNELS)
    {
        return ESP_ERR_INVALID_ARG;
    }
    List *it = schedules[channel];
    List *prev = NULL;
    //iterate through channel and try to find schedule to delete
    while (it != NULL)
    {
        if (strcmp(it->schedule.name, name) == 0)
        {
            //schedule found, delete and update next pointers
            if (prev == NULL)
            {
                //the head node needs to be deleted, free and set head to NULL
                free(it);
                schedules[channel] = NULL;
                xTaskNotify(Schedule_Task, 0, eNoAction);
                return ESP_OK;
            }
            else
            {
                //isn't head, update next pointers and free memory
                prev->next = it->next;
                free(it);
                xTaskNotify(Schedule_Task, 0, eNoAction);
                return ESP_OK;
            }
        }
        prev = it;
        it = it->next;
    }
    return ESP_ERR_NOT_FOUND;
}

esp_err_t disable_schedule_by_id(uint8_t channel, uint8_t ID)
{
    if (channel >= NUM_CHANNELS)
    {
        return ESP_ERR_INVALID_ARG;
    }

    List *it = schedules[channel];
    while (it != NULL)
    {
        if (it->schedule.ID == ID)
        {
            it->schedule.enabled = 0;
            xTaskNotify(Schedule_Task, 0, eNoAction);
            return ESP_OK;
        }
        it = it->next;
    }
    return ESP_ERR_NOT_FOUND;
}

esp_err_t disable_schedule_by_name(uint8_t channel, char *name)
{
    if (channel >= NUM_CHANNELS)
    {
        return ESP_ERR_INVALID_ARG;
    }
    List *it = schedules[channel];
    while (it != NULL)
    {
        if (strcmp(it->schedule.name, name))
        {
            it->schedule.enabled = 0;
            xTaskNotify(Schedule_Task, 0, eNoAction);
            return ESP_OK;
        }
        it = it->next;
    }
    return ESP_ERR_NOT_FOUND;
}

esp_err_t enable_schedule_by_id(uint8_t channel, uint8_t ID)
{
    if (channel >= NUM_CHANNELS)
    {
        return ESP_ERR_INVALID_ARG;
    }
    List *it = schedules[channel];
    while (it != NULL)
    {
        if (it->schedule.ID == ID)
        {
            it->schedule.enabled = 1;
            xTaskNotify(Schedule_Task, 0, eNoAction);
            return ESP_OK;
        }
        it = it->next;
    }
    return ESP_ERR_NOT_FOUND;
}

esp_err_t enable_schedule_by_name(uint8_t channel, char *name)
{
    if (channel >= NUM_CHANNELS)
    {
        return ESP_ERR_INVALID_ARG;
    }
    List *it = schedules[channel];
    while (it != NULL)
    {
        if (strcmp(it->schedule.name, name))
        {
            it->schedule.enabled = 1;
            xTaskNotify(Schedule_Task, 0, eNoAction);
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
    xTaskNotify(Schedule_Task, 0, eNoAction);
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
    xTaskNotify(Schedule_Task, 0, eNoAction);
    return ESP_OK;
}

esp_err_t delete_all_schedules(void)
{
    for (int i = 0; i < NUM_CHANNELS; ++i)
    {
        List *it = schedules[i];
        List *tmp = it;
        while (it != NULL)
        {
            tmp = it;
            it = it->next;
            free(tmp);
        }
        schedules[i] = NULL;
    }
    xTaskNotify(Schedule_Task, 0, eNoAction);
    return ESP_OK;
}

esp_err_t get_schedule_names(uint8_t channel, char* &out)
{
    if (channel >= NUM_CHANNELS)
    {
        return ESP_ERR_INVALID_ARG;
    }
    //create key/value of name and active
    //todo: json wrapper?

    //todo: should malloc correct size?
    out = (char*)malloc(sizeof(char)*1024);
    //make sure string is empty before starting
    memset(out, 0, 1024);
    int place = 0; //to keep up with where in the out buffer we are
    out[place++] = '{'; //json format is wrapped in {}
    //traverse all schedules on that channel
    List *iter = schedules[channel];
    while (iter != NULL)
    {
        int len = strlen(iter->schedule.name);
        out[place++] = '"'; //json format entries are strings, specify with ""
        for (int i = 0; i < len; i++)
        {
            out[place++] = iter->schedule.name[i]; //place name of schedule in out buffer
        }
        out[place++] = '"';
        out[place++] = ':'; //separate key/values with :
        out[place++] = iter->schedule.enabled + '0'; //convert enabled to a character
        if (iter->next != NULL)
            out[place++] = ','; //there are more entries to add, seperate with commas
        iter = iter->next;
    }
    out[place++] = '}'; //json format is wrapped in {}
    return ESP_OK;
}


esp_err_t get_schedule(uint8_t channel, char *name, schedule_object *out)
{
    if (channel >= NUM_CHANNELS)
    {
        out = NULL;
        return ESP_ERR_INVALID_ARG;
    }
    List *iter = schedules[channel];
    while (iter != NULL)
    {
        if (strcmp(iter->schedule.name, name))
        {
            *out = (iter->schedule);
            return ESP_OK;
        }
        iter = iter->next;
    }
    out = NULL;
    return ESP_ERR_NOT_FOUND;
}

void update_start_time(schedule_object *s, time_t curr)
{
    //check if schedule is repeating
    if (!s->repeat_mask && !s->repeat_time)
    {
        //was a one-time schedule. just disable
        s->enabled = 0;
        return;
    }
    //todo: revisit this logic
    //does this support lights on twice a day, once a day?
    //should repeat_time stop after day ends or carry on into next day?

    //if schedule runs every x seconds, just add to start time
    if (s->repeat_time)
    {
        s->start += s->repeat_time;
        return;
    }

    //schedule runs on certain days
    struct tm time_info;
    localtime_r(&curr, &time_info);
    //find difference between current day and next run day
    uint8_t currDay = time_info.tm_wday; //current day of week
    uint8_t nextDay = 1; //days until next run, at least 1
    for (int i = currDay + 1; i != currDay; ++i) //start with tomorrow and loop until found next day or reached back to current day
    {
        if (i > 6) i = 0; //Saturday rolls over to Sunday
        //check if schedule runs this day
        if ((s->repeat_mask >> (6-i)) & 1)
        {
            s->start += (86400*nextDay); //start time += seconds in day * num of days
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
    xTaskCreate(&Scheduler, "Scheduler", 4096, NULL, configMAX_PRIORITIES - 1, &Schedule_Task);
    return ESP_OK;
}

//task code
static void Scheduler(void *pvParms)
{
    printf("Task Started!\n");
    time_t curr;
    struct tm timeinfo;
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