#include "mqtt_app.h"
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "mqtt_client.h"
#include "timer_riego.h"
#include "config_storage.h"
#include "esp_wifi.h"
#include "esp_crt_bundle.h"

static const char *TAG = "MQTT_APP";

#define MQTT_BROKER_URI "mqtts://ff6fd9a0e6b5447db3405698ffcf1f7e.s1.eu.hivemq.cloud:8883"
#define MQTT_USER       "esp32_user"
#define MQTT_PASS       "Riego12345"

static esp_mqtt_client_handle_t client_global = NULL;

void mqtt_publicar_estado(const char *mensaje) {
    if (client_global != NULL) {
        esp_mqtt_client_publish(client_global, "riego/estado", mensaje, 0, 1, 1);
    } else {
        ESP_LOGW(TAG, "No se puede publicar (%s): Cliente MQTT aún no listo", mensaje);
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "¡CONECTADO EXITOSAMENTE AL BROKER HIVEMQ!");
            esp_mqtt_client_subscribe(event->client, "riego/config", 0);
            esp_mqtt_client_subscribe(event->client, "riego/cmd", 0);
            mqtt_publicar_estado("ESP32_CONECTADO");
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "Desconectado de MQTT, reintentando...");
            break;

        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "Mensaje en [%.*s]: %.*s", 
                     event->topic_len, event->topic,
                     event->data_len, event->data);

            if (strncmp(event->topic, "riego/config", event->topic_len) == 0) {
                int h, m, dur;
                if (sscanf(event->data, "%d,%d,%d", &h, &m, &dur) == 3) {
                    config_riego_t nueva_cfg = {
                        .hora = (uint8_t)h,
                        .minuto = (uint8_t)m,
                        .duracion_seg = (uint32_t)dur,
                        .activo = true
                    };
                    actualizar_config_riego(nueva_cfg);
                    guardar_config_riego_nvs(nueva_cfg);
                    mqtt_publicar_estado("CONFIG_ACTUALIZADA");
                }
            }
            break;

        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "Error MQTT detectado");
            break;

        default:
            break;
    }
}

void mqtt_app_start(void) {
    esp_wifi_set_ps(WIFI_PS_NONE);

    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER_URI,
        .credentials.username = MQTT_USER,
        .credentials.authentication.password = MQTT_PASS,
        .credentials.client_id = "ESP32_Riego_Station",
        
        // Usamos el Bundle oficial de certificados de ESP-IDF
        .broker.verification.crt_bundle_attach = esp_crt_bundle_attach,
        .network.timeout_ms = 15000,
    };

    client_global = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client_global, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client_global);
}