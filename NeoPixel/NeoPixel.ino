//include libraies (NeoPixel)
#include <Adafruit_NeoPixel.h>

//Neopixel Setting
#define NUMPIXELS 4
#define PIXELPIN 4

Adafruit_NeoPixel pixels(NUMPIXELS, PIXELPIN, NEO_GRB + NEO_KHZ800);  //NeoPixel Object, Please check the Neopixel Model ex> NEO_GRB

uint32_t red = pixels.Color(255,0,0);  //ColorMap
uint32_t green = pixels.Color(0,255,0);
uint32_t blue = pixels.Color(0,0,255);
uint32_t orange = pixels.Color(255,165,0);

uint32_t OFF = pixels.Color(0,0,0);
uint32_t K3000 = pixels.Color(255,180,107);
uint32_t K4000 = pixels.Color(255,209,163);
uint32_t K5000 = pixels.Color(255,228,206);
uint32_t K6000 = pixels.Color(255,243,239);

void setup()  //The function that runs when the program starts
{
  pixels.begin();
  pixels.setBrightness(10);
  Serial.begin(115200);
}

void loop()  //The funciton that runs repeatedly
{
  pixels.clear();
  pixels.show();
  pixels.setPixelColor(0,red);
  pixels.show();
  delay(250);
  
  pixels.setPixelColor(1,blue);
  pixels.show();
  delay(250);

  pixels.setPixelColor(2,green);
  pixels.show();
  delay(250);

  pixels.setPixelColor(3,orange);
  pixels.show();
  delay(250);

/*
  for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i,0,255,0);
    pixels.show();
    delay(250);
  }
*/

  delay(500);
}
