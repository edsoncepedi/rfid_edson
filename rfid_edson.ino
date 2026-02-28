#include "ID_tag.h"


void setup() 
{
  
pinMode(15, OUTPUT);
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
  
}
void loop() 
{

 //LED.show();
//Serial.println("task led\n");
vTaskDelay(50/portTICK_PERIOD_MS);

}
  