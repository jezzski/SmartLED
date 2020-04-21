#include "memory.h"

//tag for logging
static const char *TAG = "MEMORY";

//vars
bool bSPIFFS = false;
bool readNeeded = true;
char *settingsString;

esp_err_t read_settings_to_buffer(void)
{
    //buffer may not be up to date, re-read
    struct stat st;
    char src[255];
    //generate file name based on current version of firmware (in case older versions are incompatible)
    strcpy(src, "/spiffs/settings");
    strcat(src, CODE_VERSION);
    strcat(src, ".c");
    //check if file exists
    if (stat(src, &st) == 0)
    {
        FILE *f = fopen(src, "r");
        if (f == NULL)
        {
            fclose(f);
            ESP_LOGE(TAG, "failed to open settings file for reading setting!");
            return ESP_FAIL;
        }
        memset(settingsString, 0, SETTINGS_BUFFER_SIZE); //clear buffer and read file contents into
        //fscanf(f, "%s", settingsString);
        fgets(settingsString, SETTINGS_BUFFER_SIZE, f);
        fclose(f);
        return ESP_OK;
        //printf("File Contents:\n\n%s\n", buf);
    }
    else
    {
        ESP_LOGW(TAG, "couldn't find settings file. No settings can be recovered!");
        memset(settingsString, 0, SETTINGS_BUFFER_SIZE);
        return ESP_FAIL;
    }
}

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

    //allocate memory for settings buffer so we don't have to read from the file every time we get a setting
    settingsString = (char*)malloc(sizeof(char)*SETTINGS_BUFFER_SIZE);
    if (settingsString == NULL)
    {
        ESP_LOGE(TAG, "Couldn't allocate buffer for settings. Likely out of memory");
        return ESP_ERR_NO_MEM;
    }
    memset(settingsString, 0, SETTINGS_BUFFER_SIZE);

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

esp_err_t clear_schedule_data(void)
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
        ESP_LOGI(TAG, "Found schedule file, deleting");
        unlink(src);
    }
    else
    {
        ESP_LOGE(TAG, "Schedule file already deleted!");
        return ESP_OK;
    }
    
    return ESP_OK;
}

esp_err_t store_setting_string(const char *name, char *setting)
{
    if (!bSPIFFS) 
    {
        ESP_LOGE(TAG, "SPIFFS not mounted!");
        return ESP_FAIL;
    }
    //read needed, update buffer before updating
    if (readNeeded)
    {
        if (read_settings_to_buffer() != ESP_OK)
        {
            //return ESP_FAIL;
            readNeeded = true;
        }
        else
        {
            readNeeded = false;
        }
    }
    char tmp[SETTINGS_BUFFER_SIZE];
    memset(tmp, 0, SETTINGS_BUFFER_SIZE);
    strcpy(tmp, settingsString);
    //store setting in document
    StaticJsonDocument<SETTINGS_BUFFER_SIZE> doc;
    deserializeJson(doc, tmp);
    doc[name] = setting;
    //strcpy(obj[name], setting); //what is the proper way to do this? //here
    //printf("String Setting Stored: %s, %s\n", setting, obj[name]); //here
    //store new settings doc in buffer
    memset(settingsString, 0, SETTINGS_BUFFER_SIZE);
    serializeJson(doc, settingsString, SETTINGS_BUFFER_SIZE);
    //save settings to memory
    char src[255];
    strcpy(src, "/spiffs/settings");
    strcat(src, CODE_VERSION);
    strcat(src, ".c");
    
    FILE *f = fopen(src, "w");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open settings file!");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Settings file opened!");
    fprintf(f, settingsString);
    ESP_LOGI(TAG, "Serialized settings:\n %s\n", settingsString);
    fclose(f);
    doc.clear();
    readNeeded = true;
    return ESP_OK;
}

esp_err_t store_setting_int(const char *name, int setting)
{
    if (!bSPIFFS) 
    {
        ESP_LOGE(TAG, "SPIFFS not mounted!");
        return ESP_FAIL;
    }
    //read needed, update buffer before updating
    if (readNeeded)
    {
        if (read_settings_to_buffer() != ESP_OK)
        {
            //return ESP_FAIL;
            readNeeded = true;
        }
        else
        {
            readNeeded = false;
        }
    }
    char tmp[SETTINGS_BUFFER_SIZE];
    memset(tmp, 0, SETTINGS_BUFFER_SIZE);
    strcpy(tmp, settingsString);
    //store setting in document
    StaticJsonDocument<SETTINGS_BUFFER_SIZE> doc;
    deserializeJson(doc, tmp);
    doc[name] = setting;
    //printf("Int Setting Stored: %d, %d\n", setting, obj[name]); //here
    //store new settings doc in buffer
    memset(settingsString, 0, SETTINGS_BUFFER_SIZE);
    serializeJson(doc, settingsString, SETTINGS_BUFFER_SIZE);
    //save settings to memory
    char src[255];
    strcpy(src, "/spiffs/settings");
    strcat(src, CODE_VERSION);
    strcat(src, ".c");
    
    FILE *f = fopen(src, "w");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open settings file!");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Settings file opened!");
    fprintf(f, settingsString);
    ESP_LOGI(TAG, "Serialized settings:\n %s\n", settingsString);
    fclose(f);
    doc.clear();

    readNeeded = true;
    return ESP_OK;
}

esp_err_t store_setting_byte(const char *name, uint8_t setting)
{
    if (!bSPIFFS) 
    {
        ESP_LOGE(TAG, "SPIFFS not mounted!");
        return ESP_FAIL;
    }
    //read needed, update buffer before updating
    if (readNeeded)
    {
        if (read_settings_to_buffer() != ESP_OK)
        {
            //return ESP_FAIL;
            readNeeded = true;
        }
        else
        {
            readNeeded = false;
        }
    }
    char tmp[SETTINGS_BUFFER_SIZE];
    memset(tmp, 0, SETTINGS_BUFFER_SIZE);
    strcpy(tmp, settingsString);
    //store setting in document
    StaticJsonDocument<SETTINGS_BUFFER_SIZE> doc;
    deserializeJson(doc, tmp);
    doc[name] = setting;
    //printf("Byte Setting Stored: %u, %u\n", setting, obj[name]); //here
    //store new settings doc in buffer
    memset(settingsString, 0, SETTINGS_BUFFER_SIZE);
    serializeJson(doc, settingsString, SETTINGS_BUFFER_SIZE);
    //save settings to memory
    char src[255];
    strcpy(src, "/spiffs/settings");
    strcat(src, CODE_VERSION);
    strcat(src, ".c");
    
    FILE *f = fopen(src, "w");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open settings file!");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Settings file opened!");
    fprintf(f, settingsString);
    ESP_LOGI(TAG, "Serialized settings:\n %s\n", settingsString);
    fclose(f);
    doc.clear();

    readNeeded = true;
    return ESP_OK;
}

esp_err_t store_setting_double(const char *name, double setting)
{
    if (!bSPIFFS) 
    {
        ESP_LOGE(TAG, "SPIFFS not mounted!");
        return ESP_FAIL;
    }
    //read needed, update buffer before updating
    if (readNeeded)
    {
        if (read_settings_to_buffer() != ESP_OK)
        {
            //return ESP_FAIL;
            readNeeded = true;
        }
        else
        {
            readNeeded = false;
        }
    }
    char tmp[SETTINGS_BUFFER_SIZE];
    memset(tmp, 0, SETTINGS_BUFFER_SIZE);
    strcpy(tmp, settingsString);
    //store setting in document
    StaticJsonDocument<SETTINGS_BUFFER_SIZE> doc;
    deserializeJson(doc, tmp);
    doc[name] = setting;
    //printf("Double Setting Stored: %lf, %lf\n", setting, obj[name]); //here
    //store new settings doc in buffer
    memset(settingsString, 0, SETTINGS_BUFFER_SIZE);
    serializeJson(doc, settingsString, SETTINGS_BUFFER_SIZE);
    //save settings to memory
    char src[255];
    strcpy(src, "/spiffs/settings");
    strcat(src, CODE_VERSION);
    strcat(src, ".c");
    
    FILE *f = fopen(src, "w");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open settings file!");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Settings file opened!");
    fprintf(f, settingsString);
    ESP_LOGI(TAG, "Serialized settings:\n %s\n", settingsString);
    fclose(f);
    doc.clear();

    readNeeded = true;
    return ESP_OK;
}

esp_err_t get_setting_string(const char *name, char *setting)
{
    if (!bSPIFFS) 
    {
        ESP_LOGE(TAG, "SPIFFS not mounted!");
        setting = NULL;
        return ESP_FAIL;
    }
    if (readNeeded)
    {
        if (read_settings_to_buffer() != ESP_OK)
        {
            setting = NULL;
            return ESP_FAIL;
        }
        readNeeded = false;
    }
    char tmp[SETTINGS_BUFFER_SIZE];
    memset(tmp, 0, SETTINGS_BUFFER_SIZE);
    strcpy(tmp, settingsString);
    //get setting in document
    StaticJsonDocument<SETTINGS_BUFFER_SIZE> doc;
    deserializeJson(doc, tmp);
    if (doc.containsKey(name))
    {
        char buffer[250];
        serializeJson(doc, buffer);
        printf("Buffer: %s\n", buffer);
        if (!doc[name].is<char*>()) //here
        {
            ESP_LOGE(TAG, "%s is not a char*", name);
            setting = NULL;
            return ESP_FAIL;
        }
        strcpy(setting, doc[name]);
    }
    else
    {
        ESP_LOGI(TAG, "Setting %s not found", name);
        setting = NULL;
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t get_setting_int(const char *name, int *setting)
{
    if (!bSPIFFS) 
    {
        ESP_LOGE(TAG, "SPIFFS not mounted!");
        setting = NULL;
        return ESP_FAIL;
    }
    if (readNeeded)
    {
        if (read_settings_to_buffer() != ESP_OK)
        {
            setting = NULL;
            return ESP_FAIL;
        }
        readNeeded = false;
    }
    char tmp[SETTINGS_BUFFER_SIZE];
    memset(tmp, 0, SETTINGS_BUFFER_SIZE);
    strcpy(tmp, settingsString);
    //recall setting in document
    StaticJsonDocument<SETTINGS_BUFFER_SIZE> doc;
    deserializeJson(doc, tmp);
    if (doc.containsKey(name))
    {
        if (!doc[name].is<int>()) //here
        {
            ESP_LOGE(TAG, "%s is not a int*", name);
            setting = NULL;
            return ESP_FAIL;
        }
        *setting = doc[name];
    }
    else
    {
        ESP_LOGI(TAG, "Setting %s not found", name);
        setting = NULL;
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t get_setting_byte(const char *name, uint8_t *setting)
{
    if (!bSPIFFS) 
    {
        ESP_LOGE(TAG, "SPIFFS not mounted!");
        setting = NULL;
        return ESP_FAIL;
    }
    if (readNeeded)
    {
        if (read_settings_to_buffer() != ESP_OK)
        {
            setting = NULL;
            return ESP_FAIL;
        }
        readNeeded = false;
    }
    char tmp[SETTINGS_BUFFER_SIZE];
    memset(tmp, 0, SETTINGS_BUFFER_SIZE);
    strcpy(tmp, settingsString);
    //recall setting in document
    StaticJsonDocument<SETTINGS_BUFFER_SIZE> doc;
    deserializeJson(doc, tmp);
    if (doc.containsKey(name))
    {
        if (!doc[name].is<unsigned char>()) //here
        {
            ESP_LOGE(TAG, "%s is not a byte (unsigned char)", name);
            setting = NULL;
            return ESP_FAIL;
        }
        *setting = doc[name];
    }
    else
    {
        ESP_LOGI(TAG, "Setting %s not found", name);
        setting = NULL;
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t get_setting_double(const char *name, double *setting)
{
    if (!bSPIFFS) 
    {
        ESP_LOGE(TAG, "SPIFFS not mounted!");
        setting = NULL;
        return ESP_FAIL;
    }
    if (readNeeded)
    {
        if (read_settings_to_buffer() != ESP_OK)
        {
            setting = NULL;
            return ESP_FAIL;
        }
        readNeeded = false;
    }
    char tmp[SETTINGS_BUFFER_SIZE];
    memset(tmp, 0, SETTINGS_BUFFER_SIZE);
    strcpy(tmp, settingsString);
    //recall setting in document
    StaticJsonDocument<SETTINGS_BUFFER_SIZE> doc;
    deserializeJson(doc, tmp);
    if (doc.containsKey(name))
    {
        if (!doc[name].is<double>()) //here
        {
            ESP_LOGE(TAG, "%s is not a double", name);
            setting = NULL;
            return ESP_FAIL;
        }
        *setting = doc[name];
    }
    else
    {
        ESP_LOGI(TAG, "Setting %s not found", name);
        setting = NULL;
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t clear_setting_data(void)
{
    if (!bSPIFFS) 
    {
        ESP_LOGE(TAG, "SPIFFS not mounted!");
        return ESP_FAIL;
    }

    //check if files already exist
    struct stat st;
    char src[255];
    strcpy(src, "/spiffs/settings");
    strcat(src, CODE_VERSION);
    strcat(src, ".c");

    if (stat(src, &st) == 0)
    {
        ESP_LOGI(TAG, "Found setting file, deleting");
        unlink(src);
    }
    else
    {
        ESP_LOGE(TAG, "Setting file already deleted!");
        return ESP_OK;
    }
    //clear settings in ram too?
    memset(settingsString, 0, SETTINGS_BUFFER_SIZE);
    return ESP_OK;
}