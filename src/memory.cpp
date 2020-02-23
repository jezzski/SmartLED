#include "memory.h"

//tag for logging
static const char *TAG = "MEMORY";

//vars
bool bSPIFFS = false;

esp_err_t init_spiffs(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = "storage",
        .max_files = 5,
        .format_if_mount_failed = true
    };
    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        }
        else if (ret == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to init SPIFFS 9%s)", esp_err_to_name(ret));
        }
    }
    else
    {
        size_t total = 0, used = 0;
        ret = esp_spiffs_info(conf.partition_label, &total, &used);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
        }
        else
        {
            ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
        }
    }
    bSPIFFS = (ret == ESP_OK) ? true : false;
    return ret;
}


esp_err_t init_memory(void)
{
    esp_err_t ret = init_spiffs();
    if (ret != ESP_OK) return ret;
    //recall schedules by default or explicit call?
    return ESP_OK;
}

esp_err_t recall_schedules(void)
{
    if (!bSPIFFS) 
    {
        ESP_LOGE(TAG, "SPIFFS not mounted!");
        return ESP_FAIL;
    }

    //check if files already exist
    struct stat st;
    char src[255];
    strcpy(src, "/spiffs/schedules");
    strcat(src, CODE_VERSION);
    strcat(src, ".c");

    if (stat(src, &st) == 0)
    {
        ESP_LOGI(TAG, "Found schedule file, deserializing");
        FILE *f = fopen(src, "r");
        if (f == NULL)
        {
            ESP_LOGE(TAG, "Could not open file!");
            return ESP_FAIL;;
        }
        char *buf = (char*)malloc(sizeof(char)*4096);
        if (buf == NULL)
        {
            ESP_LOGE(TAG, "Could not allocate output buffer!");
            return ESP_ERR_NO_MEM;
        }
        fscanf(f, "%s", buf);
        //printf("File Contents:\n\n%s\n", buf);

        DynamicJsonDocument doc(4096);
        deserializeJson(doc, buf);
        JsonObject obj = doc.as<JsonObject>();
        printf("Key Listings:\n{\n");
        int channel = 0;
        for (auto p : obj)
        {
            printf("%s\n", p.key().c_str());
            if (p.value().is<JsonObject>())
            {
                JsonObject o = p.value();
                for (auto k : o)
                {
                    printf("-%s\n", k.key().c_str());
                    if (k.value().is<JsonObject>())
                    {
                        JsonObject j = k.value();
                        schedule_object s;
                        s.ID = j["id"];
                        strcpy(s.name, j["name"]);
                        s.enabled = j["enabled"];
                        s.start = j["start"];
                        s.duration = j["duration"];
                        s.repeat_mask = j["repeat_mask"];
                        s.repeat_time = j["repeat_time"];
                        s.isRGB = j["isRGB"];
                        s.brightness = j["brightness"];
                        s.r = j["r"];
                        s.g = j["g"];
                        s.b = j["b"];
                        create_schedule(channel, s);
                        /*for (auto test : j)
                        {
                            printf("--%s\n", test.key().c_str());
                        }*/
                    }
                }
            }
            ++channel;
        }
        printf("}\n");
        doc.clear();
        free(buf);
        fclose(f);
    }
    else
    {
        ESP_LOGE(TAG, "Schedule file does not exist!");
        return ESP_FAIL;
    }
    
    return ESP_OK;
}

esp_err_t store_schedules(void)
{
    if (!bSPIFFS) 
    {
        ESP_LOGE(TAG, "SPIFFS not mounted!");
        return ESP_FAIL;
    }

    DynamicJsonDocument doc(4096);
    for (int i = 0; i < NUM_CHANNELS; ++i) {
        char channel[4] = "CH0";
        channel[2] = i + 0x30;
        JsonObject docToObj = doc.createNestedObject(channel);
        List *iter = schedules[i];
        int schedulesStored = 0;
        while (iter != NULL)
        {
            schedule_object s = iter->schedule;
            char c = schedulesStored++ + 0x30;
            JsonObject objToStore = docToObj.createNestedObject(&c);
            objToStore["id"] = s.ID;
            objToStore["name"] = s.name;
            objToStore["enabled"] = s.enabled;
            objToStore["start"] = s.start;
            objToStore["duration"] = s.duration;
            objToStore["repeat_mask"] = s.repeat_mask;
            objToStore["repeat_time"] = s.repeat_time;
            objToStore["isRGB"] = s.isRGB;
            objToStore["brightness"] = s.brightness;
            objToStore["r"] = s.r;
            objToStore["g"] = s.g;
            objToStore["b"] = s.b;

            iter = iter->next;
        }
    }
    //todo: appropriate sizes of output?
    char *output = (char*)malloc(sizeof(char)*4096);
    memset(output, 0, 4096);
    serializeJson(doc, output, sizeof(char)*4096);
    char src[255];
    strcpy(src, "/spiffs/schedules");
    strcat(src, CODE_VERSION);
    strcat(src, ".c");

    FILE *schedFile = fopen(src, "w");
    if (schedFile == NULL)
    {
        ESP_LOGE(TAG, "Failed to open schedules file!");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Schedule file opened!");
    fprintf(schedFile, output);
    ESP_LOGI(TAG, "Serialized Schedules:\n %s\n", output);
    fclose(schedFile);
    free(output);
    doc.clear();

    return ESP_OK;
}