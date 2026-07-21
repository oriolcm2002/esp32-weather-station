#ifndef RELAY_CONTROL_H
#define RELAY_CONTROL_H

#include <stdbool.h>

// Cambia este pin por el GPIO que vayas a usar en tu placa
#define RELAY_GPIO 23

void relay_init(void);
void set_pump_state(bool turn_on);

#endif // RELAY_CONTROL_H