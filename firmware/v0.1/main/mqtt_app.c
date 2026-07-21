#include "mqtt_app.h"
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "mqtt_client.h"
#include "timer_riego.h"

static const char *TAG = "MQTT_APP";

// Cambia estos datos por los de tu Broker (ej. HiveMQ Cloud)
#define MQTT_BROKER_URI "ff6fd9a0e6b5447db3405698ffcf1f7e.s1.eu.hivemq.cloud"
#define MQTT_USER       "usuario_riego"
#define MQTT_PASS       "ClaveRiego123!"

static esp_mqtt_client_handle_t client = NULL;

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;
    
    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "¡Conectado al Broker MQTT!");
            // Nos suscribimos al tema de configuración
            esp_mqtt_client_subscribe(client, "riego/config", 0);
            break;

        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "Mensaje recibido en topic %.*s", event->topic_len, event->topic);
            ESP_LOGI(TAG, "Contenido: %.*s", event->data_len, event->data);

            // Ejemplo simple: Esperamos un formato "HORA,MINUTO,DURACION" (ejemplo: "08,15,300")
            int h, m, dur;
            if (sscanf(event->data, "%d,%d,%d", &h, &m, &dur) == 3) {
                config_riego_t nueva_cfg = {
                    .hora = (uint8_t)h,
                    .minuto = (uint8_t)m,
                    .duracion_seg = (uint32_t)dur,
                    .activo = true
                };
                actualizar_config_riego(nueva_cfg);
                ESP_LOGI(TAG, "Configuración actualizada vía MQTT");
            }
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "Desconectado de MQTT, reintentando...");
            break;

        default:
            break;
    }
}

void mqtt_app_start(void) {
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER_URI,
        .credentials.username = MQTT_USER,
        .credentials.authentication.password = MQTT_PASS,
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}


void mqtt_publicar_estado(const char *mensaje) {
    if (client != NULL) {
        // Publica en el topic 'riego/estado'
        esp_mqtt_client_publish(client, "riego/estado", mensaje, 0, 1, 0);
    }
}