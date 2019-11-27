#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

#include "lwip/apps/sntp.h"

void obtain_time(void);

//todo: sntp with old version of idf or upgrade to 4.0 somehow?