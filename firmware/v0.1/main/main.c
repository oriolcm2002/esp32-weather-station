#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_log.h"

// Módulos propios de nuestro proyecto
#include "relay_control.h"
#include "wifi_app.h"
#include "timer_riego.h"
#include "mqtt_app.h"
#include "config_storage.h"

static const char *TAG = "MAIN";

void app_main(void) {
    // 1. Inicializar la memoria Flash NVS (necesario para el Wi-Fi y para guardar datos)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 2. Inicializar los periféricos (GPIO del relé)
    relay_init();

    // 3. Conectar a la red Wi-Fi
    wifi_init_sta();

    // Pequeña espera para asegurar que la pila TCP/IP reciba una dirección IP válida
    vTaskDelay(pdMS_TO_TICKS(2000));

    // 4. Cargar configuración guardada de riego en la Flash NVS
    config_riego_t mi_riego;
    if (!cargar_config_riego_nvs(&mi_riego)) {
        // Si es la primera vez que se enciende o la NVS está vacía, usa la configuración por defecto
        ESP_LOGI(TAG, "NVS vacía. Cargando configuración por defecto (07:30 AM - 300s).");
        mi_riego.hora = 7;
        mi_riego.minuto = 30;
        mi_riego.duracion_seg = 300;
        mi_riego.activo = true;
    }

    // 5. Iniciar la tarea de sincronización de hora NTP y verificación de riego
    timer_riego_init(mi_riego);

    // 6. Arrancar la aplicación MQTT para recibir comandos desde EasyMQTT / HiveMQ
    mqtt_app_start();

    ESP_LOGI(TAG, "¡Sistema de riego automatizado con persistencia NVS y MQTT listo!");
}