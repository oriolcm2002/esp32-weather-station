#include "relay_control.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "RELAY";

void relay_init(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << RELAY_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    // Arrancar con la bomba apagada por seguridad
    gpio_set_level(RELAY_GPIO, 0);
    ESP_LOGI(TAG, "Relé inicializado en GPIO %d", RELAY_GPIO);
}

void set_pump_state(bool turn_on) {
    gpio_set_level(RELAY_GPIO, turn_on ? 1 : 0);
    ESP_LOGI(TAG, "Bomba estado: %s", turn_on ? "ENCENDIDA" : "APAGADA");
}