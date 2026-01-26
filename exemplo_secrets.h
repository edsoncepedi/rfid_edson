#pragma once

#define WIFI_SSID ""
#define WIFI_PASS ""

#define API_TOKEN_GIT_HUB ""
#define API_URL_GIT_HUB ""

#define PAT_AZURE ""
#define API_URL_AZURE ""

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define POSTO_ID 1
#define USER_MQTT "posto_" STR(POSTO_ID)
#define MQTT_TOPIC "rastreio_nfc/esp32/posto_" STR(POSTO_ID) "/dispositivo"
#define IP_TOPIC "rastreio/esp32/posto_" STR(POSTO_ID) "/ip"
#define MAC_TOPIC "rastreio/esp32/posto_" STR(POSTO_ID) "/mac"
#define SISTEMA_TOPIC "rastreio/esp32/posto_" STR(POSTO_ID) "/sistema"

//Deve-se criar o arquivo secrets.h preenchendo as definições. Esse arquivo é necessário para compilar o projeto.