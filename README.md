# esp32-weather-station
ESP32-based IoT weather station with MQTT connectivity and automatic irrigation.

## <u><strong>MQTT</strong></u>

### Consultar el estado actual
- Topic: `riego/cmd`
- Enviar: `GET`
- El estado se publica en: `riego/estado`

### Cambiar el horario de riego
- Publicar: `20,15,60` (Regar a las 20:15h, durante 60 segundos)
- Topic: `riego/config`
