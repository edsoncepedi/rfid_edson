#include <stdint.h>
#include "WString.h"
#include <PubSubClient.h>  
#include <WiFi.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <HTTPClient.h>
#include "esp_task_wdt.h"
#include <ArduinoJson.h>
#include "secrets.h"
#include "base64.h"


TaskHandle_t reconnect_Task; 
TaskHandle_t envio_Task; 
QueueHandle_t mqttQueue;


//estrutura do cartao
struct cartao{

  uint8_t ID;
  status  ESTADO; 
  uint8_t tempo=0;
  bool LIDO = false;
  char TAG[16];
  char value[16]="";
  uint8_t verificacao=0;
};

  


cartao CARTAO; //struct cartao

void callback(char* topic, byte* message, unsigned int length);
void taks_ManterConexao(void* pVParams);
void task_enviarDados(void* pvParams);
void reconnect();
void envia_dispositivo(char* mensagem, char* topico);
void baixarConfigPrivada();


char msg[50];    // Buffer para armazenar a mensagem
char topic[50];  //Topico enviar dado

typedef struct {
  char topic[64];
  char payload[256];
} mqtt_message_t;

/* ======================================== SSID e Senha da Wifi */
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;

String apiUrl = API_URL_AZURE;
const char* patToken = PAT_AZURE;

String mqtt_server = "172.16.10.184";
int mqtt_port = 1883;

//Declarando objeto Wifi
WiFiClient espClient; // Cliente Wi-Fi para comunicação com o broker MQTT

//Declarando objeto MQTT
PubSubClient client(espClient); // Cliente MQTT usando o cliente Wi-Fi



void setup_mqtt(){
  WiFi.mode(WIFI_STA);
  Serial.println("------------");
  Serial.print("Conectando-se a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  int connecting_process_timed_out = 20; //--> 20 = 20 seconds.
  connecting_process_timed_out = connecting_process_timed_out * 2;

  while (WiFi.status() != WL_CONNECTED) {
    //Flash pisca até que a Wifi esteja conectada
    Serial.print(".");
    vTaskDelay(250);

    if(connecting_process_timed_out > 0) connecting_process_timed_out--;
    // Se o tempo de timeout for superado sem conexão o Esp será reiniciado
    if(connecting_process_timed_out == 0) {
      vTaskDelay(100);
      ESP.restart();
    }
  }
  
  baixarConfigPrivada();

  client.setServer(mqtt_server.c_str(), mqtt_port);
  client.setCallback(callback);

  //Tasks
  mqttQueue = xQueueCreate(30, sizeof(mqtt_message_t));  // até 30 mensagens
  xTaskCreatePinnedToCore(&task_enviarDados, "task_enviarDados", 8192, NULL, 5, NULL, 1);
  xTaskCreatePinnedToCore(&taks_ManterConexao, "taks_ManterConexao", 8192, NULL, 4, NULL, 1);
}

// Função de callback que é chamada sempre que uma nova mensagem chega a um tópico inscrito
void callback(char* topic, byte* message, unsigned int length) {
  uint8_t posto;

  Serial.print("Messagem chegou ao topico: ");  // Imprime o tópico
  Serial.print(topic);
  Serial.print("\nMessagem: ");
  
  String messageTemp = "";  // String temporária para armazenar a mensagem recebida

  posto = message[0]-48;
  // Imprime e armazena os dados da mensagem
  for (int i = 0; i < length; i++) { 
    messageTemp += (char)message[i]; // Adiciona o byte à string messageTemp
  }
  Serial.println(messageTemp);
  Serial.println(posto);

/*  Permitir o envio manual apenas quando solicitado no proprio posto*/


  /** Enviar ID ao ser solicitado **/
  envia_dispositivo(CARTAO.value/*TAG*/ , MQTT_TOPIC); //envia tag

   // Serial.println(topic[29]);

  

}
void taks_ManterConexao(void* pVParams) 
{
  while (1) 
  {
    client.loop();  // Mantém a comunicação com o broker MQTT
      // Verifica se o cliente MQTT está conectado, caso contrário tenta reconectar
    if (!client.connected()) 
    {
      reconnect();
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void task_enviarDados(void* pvParams) {
  mqtt_message_t msg;

  while (1) {
    if (xQueueReceive(mqttQueue, &msg, portMAX_DELAY)) {
      client.loop();

      if (!client.connected()) {
        reconnect();
      }

      if (client.connected()) {
        client.publish(msg.topic, msg.payload);
        Serial.printf("Publicado: [%s] %s\n", msg.topic, msg.payload);
      }
    }
  }
}

void baixarConfigPrivada() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String auth = ":" + String(patToken);               // Atenção: ':' antes do PAT
    auth = base64::encode(auth);                        // Codifica em Base64
    String headerAuth = "Basic " + auth;

    http.begin(apiUrl);
    http.addHeader("User-Agent", "ESP32");
    http.addHeader("Authorization", headerAuth);
    http.addHeader("Cache-Control", "no-cache");
    http.addHeader("Pragma", "no-cache"); 

    int httpCode = http.GET();

    if (httpCode == 200) {
      String response = http.getString();

      Serial.println("Resposta da API:");
      Serial.println(response);

      // Interpretar JSON
      StaticJsonDocument<1024> doc;
      DeserializationError err = deserializeJson(doc, response);

      if (err) {
        Serial.print("Erro ao interpretar JSON: ");
        Serial.println(err.c_str());
        return;
      }

      mqtt_server = doc["mqtt_server"].as<String>(); 
      mqtt_port = doc["mqtt_port"].as<int>();


      // Debug
      Serial.print("mqtt_server: ");
      Serial.println(mqtt_server);

      Serial.print("mqtt_port: ");
      Serial.println(mqtt_port);


    } else {
      Serial.printf("Erro HTTP: %d\n", httpCode);
    }

    http.end();
  }
}

// Função para reconectar ao broker MQTT caso a conexão seja perdida
void reconnect() {
    while (!client.connected()) 
    {
      Serial.print("Attempting MQTT connection...");
      // Tenta conectar com o ID "camera1", informação de usuário e senha para o broker mqtt
      if (client.connect(USER_MQTT, USER_MQTT, "cepedi123"))
      {
       // client.subscribe("rastreio/esp32/posto_0/sistema");
       
        
        Serial.println("connected");     // Se a conexão for bem-sucedida
      } 
      else 
      {
        // Caso falhe a conexão, imprime o código de erro e tenta novamente após 5 segundos
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        vTaskDelay( 5000 / portTICK_PERIOD_MS );  // Espera 5 segundos antes de tentar novamente
      }
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }

void envia_dispositivo(char* mensagem, char* topico){
  mqtt_message_t msg;

  strncpy(msg.topic, topico, sizeof(msg.topic));
  msg.topic[sizeof(msg.topic) - 1] = '\0'; // garante null-termination

  strncpy(msg.payload, mensagem, sizeof(msg.payload));
  msg.payload[sizeof(msg.payload) - 1] = '\0';

  xQueueSend(mqttQueue, &msg, portMAX_DELAY);
}