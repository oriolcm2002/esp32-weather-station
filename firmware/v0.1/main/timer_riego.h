#ifndef TIMER_RIEGO_H
#define TIMER_RIEGO_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t hora;            // Hora de inicio (0 - 23)
    uint8_t minuto;          // Minuto de inicio (0 - 59)
    uint32_t duracion_seg;   // Tiempo que permanecerá encendida la bomba en segundos
    bool activo;             // Permite activar/desactivar la programación
} config_riego_t;

// Inicializa NTP y lanza la tarea de verificación
void timer_riego_init(config_riego_t config_inicial);

// Permite cambiar la configuración en tiempo de ejecución
void actualizar_config_riego(config_riego_t nueva_config);

#endif // TIMER_RIEGO_H