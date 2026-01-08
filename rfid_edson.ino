#include "ID_tag.h"


void setup() 
{
  
pinMode(15, OUTPUT);
pinMode(4, OUTPUT); //dados led
pinMode(2, INPUT_PULLUP);
  

  IniciarRFID();
  //LED.begin(); // iniciar led
  //LED.clear(); //deslidar leds

	delay(200);
  setup_mqtt();
  delay(100);
	Serial.begin(115200);		// Initialize serial communications with the PC
	

	// void SPIClass::begin(sck, miso, mosi, ss)
	SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);			// Init SPI bus
	mfrc522.PCD_Init();		// Init MFRC522
	delay(40);				// Optional delay. Some board do need more time after init to be ready, see Readme
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  Serial.println("Aproximar o cartao ao leitor...");

  //**************** TASK PARA LER CARTAO ********************///
  
  
  xTaskCreatePinnedToCore(
    LerID,         // Task function
    "LerID_task",       // Task name
   10000,             // Stack size (bytes)
    NULL,              // Parameters
    5,                 // Priority
    &LerIDHandle,  // Task handle
    0                  // Core 0
  );
  
  // Essa task é responsável por monitorar o comportamento dos botões
  xTaskCreatePinnedToCore(
    monitoramento_Parafusadeira,          /* Função da Task. */
    "monitoramento_Parafusadeira",        /* Nome da Task. */
    2048,           /* Memória destinada a Task */
    NULL,           /* Parâmetro para Task */
    3,              /* Nível de prioridade da Task */
    &monitoramento_Parafusadeira_Task,    /* Handle da Task */
    0);             /* Núcleo onde a task é executada 0 ou 1 */
  /* ---------------------------------------- */ 

  
}
void loop() 
{

 //LED.show();
//Serial.println("task led\n");
vTaskDelay(50/portTICK_PERIOD_MS);

}
  