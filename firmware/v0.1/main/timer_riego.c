#include "timer_riego.h"
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sntp.h"      // Esta funciona perfectamente cuando 'lwip' está en el CMakeLists.txt
#include "esp_log.h"
#include "relay_control.h"
#include "mqtt_app.h"

static const char *TAG = "TIMER_RIEGO";

static config_riego_t config_actual;
static bool regando_hoy = false; // Evita que se active múltiples veces dentro del mismo minuto

// Configurar hora con servidores NTP y zona horaria (España/Europa por defecto)
static void obtener_hora_sntp(void) {
    ESP_LOGI(TAG, "Inicializando cliente SNTP...");
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");
    esp_sntp_init();

    // Establecer Zona Horaria para España/Europa Central (Ajusta si estás en otro país)
    // CET-1CEST,M3.5.0,M10.5.0/3 aplica automáticamente el horario de verano/invierno
    setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
    tzset();
}

// Tarea en segundo plano que revisa la hora cada 10 segundos
static void tarea_verificar_riego(void *pvParameters) {
    time_t now;
    struct tm timeinfo;

    // Esperar a que la hora esté sincronizada vía NTP
    ESP_LOGI(TAG, "Esperando sincronización de hora...");
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET) {
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
    ESP_LOGI(TAG, "¡Hora NTP sincronizada exitosamente!");

    while (1) {
        time(&now);
        localtime_r(&now, &timeinfo);

        // Imprime la hora local en los logs para monitorear
        // ESP_LOGI(TAG, "Hora actual: %02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

        if (config_actual.activo) {
            // Comprobar si coincide la hora y el minuto
            if (timeinfo.tm_hour == config_actual.hora && timeinfo.tm_min == config_actual.minuto) {
                if (!regando_hoy) {
                    regando_hoy = true;
                    ESP_LOGI(TAG, "¡Iniciando ciclo de riego programado por %lu segundos!", config_actual.duracion_seg);
                    
                    set_pump_state(true); // Encender bomba
                    mqtt_publicar_estado("BOMBA_ON"); // <--- Avisa al móvil de que empezó a regar
                    
                    // Esperar la duración del riego
                    vTaskDelay(pdMS_TO_TICKS(config_actual.duracion_seg * 1000));
                    
                    set_pump_state(false); // Apagar bomba
                    ESP_LOGI(TAG, "Ciclo de riego finalizado.");
                    mqtt_publicar_estado("BOMBA_OFF"); // <--- Avisa al móvil de que terminó
                }
            } else {
                // Resetear la bandera cuando salimos del minuto programado
                regando_hoy = false;
            }
        }

        // Revisar cada 10 segundos para no saturar la CPU
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

void timer_riego_init(config_riego_t config_inicial) {
    config_actual = config_inicial;
    obtener_hora_sntp();

    // Crear la tarea FreeRTOS en segundo plano
    xTaskCreate(tarea_verificar_riego, "tarea_verificar_riego", 4096, NULL, 5, NULL);
}

void actualizar_config_riego(config_riego_t nueva_config) {
    config_actual = nueva_config;
    ESP_LOGI(TAG, "Nueva configuración: %02d:%02d por %lu seg (%s)",
             config_actual.hora, config_actual.minuto, config_actual.duracion_seg,
             config_actual.activo ? "Activo" : "Inactivo");
}