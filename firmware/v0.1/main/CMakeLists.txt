idf_component_register(SRCS "main.c" "relay_control.c" "wifi_app.c" "timer_riego.c" "mqtt_app.c" "config_storage.c"
                       INCLUDE_DIRS "."
                       REQUIRES nvs_flash esp_wifi driver lwip esp_netif mqtt)