#include <Adafruit_NeoPixel.h>



//ESTADO DO LEITOR
typedef enum {

  RODANDO,  //azul
  LEITURA,  //verde
  PRESENTE, //laranja
  LIVRE,    //branco

}status;

//Configuração led
#define PIN_LED 2
#define N_PIXELS 3

//Adafruit_NeoPixel LED(N_PIXELS, PIN_LED, NEO_GRB + NEO_KHZ800);


//definições para cor
#define VERDE LED.Color(0,255,0)
#define BRANCO LED.Color(255,255,255)
#define LARANJA LED.Color(255,160,0)
#define AZUL LED.Color(0,0,255)

//Numero do led e cor
#define S_LED(x,y)  LED.setPixelColor(x-1,y);

