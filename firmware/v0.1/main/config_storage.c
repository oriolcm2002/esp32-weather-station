#include "config_storage.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"

static const char *TAG = "STORAGE";
#define NVS_NAMESPACE "riego_ns"

esp_err_t guardar_config_riego_nvs(config_riego_t cfg) {
    nvs_handle_t my_handle;
    esp_err_t err;

    // Abrir el espacio de nombres en modo Lectura/Escritura
    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) abriendo NVS para escribir", esp_err_to_name(err));
        return err;
    }

    // Escribir los valores
    nvs_set_u8(my_handle, "hora", cfg.hora);
    nvs_set_u8(my_handle, "minuto", cfg.minuto);
    nvs_set_u32(my_handle, "duracion", cfg.duracion_seg);
    nvs_set_u8(my_handle, "activo", cfg.activo ? 1 : 0);

    // Confirmar cambios en la memoria Flash
    err = nvs_commit(my_handle);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Configuración de riego guardada en NVS correctamente.");
    }

    // Cerrar el manejador
    nvs_close(my_handle);
    return err;
}

bool cargar_config_riego_nvs(config_riego_t *cfg) {
    nvs_handle_t my_handle;
    esp_err_t err;

    // Abrir espacio de nombres en modo Solo Lectura
    err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &my_handle);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "No se encontró configuración previa en NVS (Usando valores por defecto).");
        return false;
    }

    uint8_t h = 0, m = 0, act = 0;
    uint32_t dur = 0;

    // Leer valores guardados
    err = nvs_get_u8(my_handle, "hora", &h);
    err |= nvs_get_u8(my_handle, "minuto", &m);
    err |= nvs_get_u32(my_handle, "duracion", &dur);
    err |= nvs_get_u8(my_handle, "activo", &act);

    nvs_close(my_handle);

    if (err == ESP_OK) {
        cfg->hora = h;
        cfg->minuto = m;
        cfg->duracion_seg = dur;
        cfg->activo = (act == 1);
        ESP_LOGI(TAG, "Configuración cargada desde NVS: %02d:%02d por %lu seg", h, m, dur);
        return true;
    }

    return false;
}