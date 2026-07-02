#include "system.h"
#include "config.h"

#include <inttypes.h>

#include "esp_chip_info.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_system.h"

static const char *TAG = "SYSTEM";

void system_print_banner(void)
{
    esp_chip_info_t chip_info;

    esp_chip_info(&chip_info);

    ESP_LOGI(TAG, "=====================================");
    ESP_LOGI(TAG, "%s", PROJECT_NAME);
    ESP_LOGI(TAG, "Version %s", PROJECT_VERSION);
    ESP_LOGI(TAG, "=====================================");

    ESP_LOGI(TAG, "Chip Revision : %d", chip_info.revision);
    ESP_LOGI(TAG, "CPU Cores     : %d", chip_info.cores);
    ESP_LOGI(TAG, "Free Heap     : %" PRIu32 " bytes",
             esp_get_free_heap_size());

    ESP_LOGI(TAG, "System initialized successfully");
}