#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "system.h"
#include "wifi.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    system_print_banner();

    ESP_LOGI(TAG, "Initializing WiFi...");

    wifi_init();

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}