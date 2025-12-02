#include <stdint.h>
#include "WString.h"
#include <PubSubClient.h>  
#include <WiFi.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


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
void envia_dispositivo(char* mensagem, char* topico, uint8_t numero);

int numero_posto = 20;

char topico_dispositivo[40];
char topico_sistema[35];
char id_posto[15];

char msg[50];    // Buffer para armazenar a mensagem
char topic[50];  //Topico enviar dado

typedef struct {
  char topic[64];
  char payload[256];
} mqtt_message_t;

/* ======================================== SSID e Senha da Wifi */
const char* ssid = "gemeodigital";
const char* password = "gd@2025p";

const char* mqtt_server = "172.16.10.175";

//Declarando objeto Wifi
WiFiClient espClient; // Cliente Wi-Fi para comunicação com o broker MQTT

//Declarando objeto MQTT
PubSubClient client(mqtt_server, 1883, callback, espClient); // Cliente MQTT usando o cliente Wi-Fi



void setup_mqtt(){
  snprintf(topico_dispositivo, sizeof(topico_dispositivo), "rastreio_nfc/esp32/posto_%d/dispositivo", numero_posto);
  snprintf(topico_sistema, sizeof(topico_sistema), "rastreio_nfc/esp32/posto_%d/sistema", numero_posto);
  snprintf(id_posto, sizeof(id_posto), "posto_%d", numero_posto);

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
  envia_dispositivo(CARTAO.value/*TAG*/ , "rastreio/esp32/posto__/dispositivo",posto); //envia tag

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

// Função para reconectar ao broker MQTT caso a conexão seja perdida
void reconnect() {
    while (!client.connected()) 
    {
      Serial.print("Attempting MQTT connection...");
      // Tenta conectar com o ID "camera1", informação de usuário e senha para o broker mqtt
      if (client.connect(id_posto, id_posto, "cepedi123"))
      {
        client.subscribe("rastreio/esp32/posto_0/sistema");
       
        
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

void envia_dispositivo(char* mensagem, char* topico,uint8_t numero){
  mqtt_message_t msg;

  strncpy(msg.topic, topico, sizeof(msg.topic));
  msg.topic[21] = numero+48; //substituir e converter o numero para char
  msg.topic[sizeof(msg.topic) - 1] = '\0'; // garante null-termination

  strncpy(msg.payload, mensagem, sizeof(msg.payload));
  msg.payload[sizeof(msg.payload) - 1] = '\0';

  xQueueSend(mqttQueue, &msg, portMAX_DELAY);
}