#include "freertos/FreeRTOS.h"
#include "HardwareSerial.h"
#include "esp32-hal.h"
//#include "freertos/FreeRTOS.h"
#include "esp32-hal-gpio.h"
#include "esp32-hal-spi.h"
#include <stdint.h>
#include <SPI.h>
#include <MFRC522.h>
#include "LED.h"
#include "mqtt_comunicacao.h"
#include "secrets.h"

TaskHandle_t LerIDHandle = NULL;
TaskHandle_t monitoramento_Parafusadeira_Task;

//pinos de dados
#define MISO_PIN        12
#define MOSI_PIN        13
#define SCK_PIN         14
#define SS_PIN          15
#define RST_PIN         19
#define parafusadeira   2 
//#define N_LEITOR 3 //quantidade de leitores


MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
MFRC522::MIFARE_Key key;
MFRC522::StatusCode status_rfid;

byte bufferATQA[3]; // Buffer to store the ATQA response
byte bufferSize = sizeof(bufferATQA); // Size of the buffer


void mfrc522_fast_Reset(uint8_t reader);


String content;//= ""; armazenar conteudo



void LerID(void *parameter){

  while(1){
  // Look for new cards
    mfrc522_fast_Reset(0); //************ FUNÇÃO PARA RESETAR LEITOR PARA LER CONTINUAMENTE **************************//
    vTaskDelay(50/portTICK_RATE_MS);
       

  if ( (mfrc522.PICC_IsNewCardPresent())){// && mfrc522[j].PICC_ReadCardSerial()) ) 
        mfrc522.PICC_ReadCardSerial(); //ler cartao
 
 //mfrc522[j].PICC_WakeupA(bufferATQA, &bufferSize); //ficar lendo direto
       // vTaskDelay(50/portTICK_RATE_MS);
        // for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  
        if(CARTAO.tempo>=5)
    {
      CARTAO.ESTADO = PRESENTE;
    }
    else{
      
    CARTAO.tempo++;
    CARTAO.ESTADO = LEITURA;
    }
/*
    
  //Serial.print("Card presente\nTempo decorrido: ");
  //Serial.println(CARTAO[j].tempo);
*/






  if((CARTAO.ESTADO == LEITURA) && (CARTAO.LIDO == false)){ //verificar quando o cartao e lido

    CARTAO.LIDO = true;
    Serial.println("Cartao Lido");



//armazenar id na string

    content = ""; //limpar string

//**************** SALVAR ID    *********************//

  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     //Serial.print(mfrc522[j].uid.uidByte[i] < 0x10 ? " 0" : " ");
     //Serial.print(mfrc522[j].uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  //**************** SALVAR ID    *********************//

  Serial.println();
  Serial.print("Leitor nº: ");
  content.toUpperCase();  

  
  content.toCharArray(CARTAO.value, sizeof(CARTAO.value)); //converter String para char

    //**teste ler valor gravado no card *****************
/*
        status_rfid = mfrc522[j].PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522[j].uid));
    
        vTaskDelay(20/portTICK_RATE_MS);

    
        if (status_rfid != MFRC522::STATUS_OK) {
          Serial.print(F("Authentication failed: "));
          Serial.println(mfrc522[j].GetStatusCodeName(status_rfid));
         // return;
        }
        status_rfid = mfrc522[j].MIFARE_Read(block, buffer1, &len);
        if (status_rfid != MFRC522::STATUS_OK) {
          Serial.print(F("Reading failed: "));
          Serial.println(mfrc522[j].GetStatusCodeName(status_rfid));
          //return;
        }


*/
//************************************************************************//
 //mfrc522[j].PICC_WakeupA(bufferATQA, &bufferSize); //ficar lendo direto
   //     vTaskDelay(50/portTICK_RATE_MS);
       
//************************************************************************//

/*

        //fazer a conversao do valor lido
        for (uint8_t i = 0; i < 6; i++) {
          content[j].concat(String((char)buffer1[i]));
        }

        content[j].toCharArray(CARTAO[j].value, 6); //converter String para array
        
        Serial.print(CARTAO[j].value);
        content[j] = ""; //limpar string
  
  */
//*********************************/

    

     envia_dispositivo( "BS" , MQTT_TOPIC); //enviar chegada



  envia_dispositivo(CARTAO.value/*TAG*/ , MQTT_TOPIC); //envia tag

    digitalWrite(4, HIGH);
    vTaskDelay(500);
    digitalWrite(4, LOW);
  }
 
 //mfrc522[j].PICC_WakeupA(bufferATQA, &bufferSize); //ficar lendo direto



    CARTAO.verificacao = 0;

  }
 

  else if((!(mfrc522.PICC_IsNewCardPresent())) && ((CARTAO.ESTADO == PRESENTE) || CARTAO.ESTADO == LEITURA)){ //verificar quando o cartao e removido

    Serial.println("Cartão removido");

    
    
    CARTAO.ESTADO = LIVRE;
    CARTAO.tempo = 0;
    CARTAO.LIDO = false;
    CARTAO.verificacao = 0;

    envia_dispositivo( "BD" , MQTT_TOPIC); //envia saida


  }
   else if(!(mfrc522.PICC_IsNewCardPresent()) && ((CARTAO.ESTADO == PRESENTE) || CARTAO.ESTADO == LEITURA)){

    content = "";
    content.toCharArray(CARTAO.value, sizeof(CARTAO.value)); //limpar string
  }



  else{ //se não ler deixar livre e limpar
    CARTAO.ESTADO = LIVRE;
    CARTAO.tempo = 0;
    CARTAO.LIDO = false;

    
    CARTAO.verificacao = 0;

    content = "";
    content.toCharArray(CARTAO.value, sizeof(CARTAO.value)); //limpar string
  }
 





  
    mfrc522.PCD_StopCrypto1();

/*
  Serial.print("Verificação: ");
  Serial.println(CARTAO[j].verificacao);

*/




  
 /*
  // Halt PICC
   for (uint8_t j=0; j<N_LEITOR; j++){
  //mfrc522[j].PICC_HaltA();
   // vTaskDelay( 10 / portTICK_PERIOD_MS );  // 
  }

  // Stop encryption on PCD
    for (uint8_t j=0; j<N_LEITOR; j++){
  //mfrc522[j].PCD_StopCrypto1();
    // vTaskDelay( 10 / portTICK_PERIOD_MS );  // 
 }
 */
      //Serial.printf("Task1 Stack Free: %u bytes\n", uxTaskGetStackHighWaterMark(NULL));
        vTaskDelay( 50 / portTICK_PERIOD_MS );  // 



}//end while task


} //end LerID; 


void IniciarRFID(){



	SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);			// Init SPI bus
	mfrc522.PCD_Init();		// Init MFRC522
	delay(40);				// Optional delay. Some board do need more time after init to be ready, see Readme
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  
}// end IniciarRFID


//************ FUNÇÃO PARA RESETAR LEITOR PARA LER CONTINUAMENTE **************************//
void mfrc522_fast_Reset(uint8_t reader) {
  digitalWrite(RST_PIN, HIGH);
  mfrc522.PCD_Reset();
  mfrc522.PCD_WriteRegister(mfrc522.TModeReg, 0x80);      // TAuto=1; timer starts automatically at the end of the transmission in all communication modes at all speeds
  mfrc522.PCD_WriteRegister(mfrc522.TPrescalerReg, 0x43);   // 10μs.43
  //  mfrc522.PCD_WriteRegister(mfrc522.TPrescalerReg, 0x20);   // test

  mfrc522.PCD_WriteRegister(mfrc522.TReloadRegH, 0x00);   // Reload timer with 0x064 = 30, ie 0.3ms before timeout.
  mfrc522.PCD_WriteRegister(mfrc522.TReloadRegL, 0x3C); //1E
  //  mfrc522.PCD_WriteRegister(mfrc522.TReloadRegL, 0x1E);

  mfrc522.PCD_WriteRegister(mfrc522.TxASKReg, 0x40);    // Default 0x00. Force a 100 % ASK modulation independent of the ModGsPReg register setting
  mfrc522.PCD_WriteRegister(mfrc522.ModeReg, 0x3D);   // Default 0x3F. Set the preset value for the CRC coprocessor for the CalcCRC command to 0x6363 (ISO 14443-3 part 6.2.4)

  mfrc522.PCD_AntennaOn();            // Enable the antenna driver pins TX1 and TX2 (they were disabled by the reset)
}

bool leituraAnterior_parafusadeira = true;    
bool estadoEstavel_parafusadeira = false;             // Estado final confiável
const int tempoDebounce_parafusadeira = 100;
unsigned long tempoUltimaLeitura_parafusadeira = 0;  // Quando a mudança começou

void monitoramento_Parafusadeira ( void * pvParameters ) {
  while(1){
    bool leituraAtual = digitalRead(parafusadeira);

    // Se a leitura mudou em relação à anterior
    if (leituraAtual != leituraAnterior_parafusadeira) {
      leituraAnterior_parafusadeira = leituraAtual;
      tempoUltimaLeitura_parafusadeira = millis();  // Marca o tempo da mudança
    }

    // Se o tempo de estabilidade foi atingido
    if ((millis() - tempoUltimaLeitura_parafusadeira) > tempoDebounce_parafusadeira) {
      // E se o estado ainda não foi atualizado
      if (leituraAtual != estadoEstavel_parafusadeira) {
        // Aqui temos uma mudança de borda real, com debounce
        bool estadoAnterior = estadoEstavel_parafusadeira;
        estadoEstavel_parafusadeira = leituraAtual;

        if (estadoEstavel_parafusadeira && !estadoAnterior) {
          Serial.println("BT2");
          envia_dispositivo("BT2", MQTT_TOPIC);
        } 
        else if (!estadoEstavel_parafusadeira && estadoAnterior) {
          Serial.println("BT1");
          envia_dispositivo("BT1", MQTT_TOPIC);
        }
      }
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
