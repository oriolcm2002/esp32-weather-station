#ifndef CONFIG_STORAGE_H
#define CONFIG_STORAGE_H

#include <stdbool.h>
#include "esp_err.h"       // <--- AÑADE ESTA LÍNEA

#include "timer_riego.h"

// Guarda la estructura config_riego_t en la Flash
esp_err_t guardar_config_riego_nvs(config_riego_t cfg);

// Carga la estructura config_riego_t desde la Flash. Retorna true si encontró datos previos.
bool cargar_config_riego_nvs(config_riego_t *cfg);

#endif // CONFIG_STORAGE_H