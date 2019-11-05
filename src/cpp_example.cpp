/*#include <string>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

extern "C" {
    void app_main(void);
}

class Test {
private:
    std::string msg;
public:
    Test() {
        this->msg = "Hello!";
     }
    void print(void)
    {
        printf("Test2:%s\n", msg.c_str());
    }
};

void hello_task(void *pvParm)
{
    Test tst;
    while (true)
    {
        tst.print();
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
    vTaskDelete(NULL);
}

void app_main()
{
    xTaskCreate(&hello_task, "hello_task", 2048, NULL, 5, NULL);
}*/