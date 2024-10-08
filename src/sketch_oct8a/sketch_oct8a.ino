/*****************  NEEDED TO MAKE NODEMCU WORK ***************************/
#define FASTLED_INTERRUPT_RETRY_COUNT 0
#define FASTLED_ESP8266_RAW_PIN_ORDER
/******************  LIBRARY SECTION *************************************/
#include <FastLED.h>
/*****************  LED LAYOUT AND SETUP *********************************/
#define NUM_LEDS 60 //+3
/*****************  DECLARATIONS  ****************************************/
CRGB leds[NUM_LEDS];
/*****************  GLOBAL VARIABLES  ************************************/
const int ledPin = 4; // marked as D2 on the board

int idx = 1;
/*****************  SETUP FUNCTIONS  ****************************************/
void setup()
{

  Serial.begin(9600);
  FastLED.addLeds<WS2813, ledPin, GRB>(leds, NUM_LEDS);
}
/*****************  MAIN LOOP  ****************************************/
void loop()
{
  if (idx >= NUM_LEDS)
  {
    idx = 1;
  }
  if (leds[idx].r > 0)
  {
    leds[idx] = CRGB(0, 0, 0);
  }
  else
  {
    leds[idx] = CRGB(255, 255, 0);
  }
  idx = idx + 2;

  FastLED.setBrightness(25);
  FastLED.show();
  delay(75);
}