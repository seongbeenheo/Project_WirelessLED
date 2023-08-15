//include libraies (NeoPixel)
#include <Adafruit_NeoPixel.h>

//Neopixel Setting
#define NUMPIXELS 4
#define PIXELPIN 2

Adafruit_NeoPixel pixels(NUMPIXELS, PIXELPIN, NEO_GRB + NEO_KHZ800);  //NeoPixel Object, Please check the Neopixel Model ex> NEO_GRB

uint32_t red = pixels.Color(255,0,0);  //ColorMap
uint32_t green = pixels.Color(0,255,0);
uint32_t blue = pixels.Color(0,0,255);
uint32_t orange = pixels.Color(255,165,0);

void setup()  //The function that runs when the program starts
{
  pixels.begin();
  pixels.setBrightness(255);
  Serial.begin(115200);
}

void loop()  //The funciton that runs repeatedly
{
  pixels.clear();
  pixels.show();
  Serial.println(bright);
  if(bright<50) {
    for(int i=0; i<NUMPIXELS; i++) {
      pixels.setPixelColor(i,blue);
      pixels.show();
      delay(1000);
    }
  } else if(bright<100) {
    for(int i=0; i<NUMPIXELS; i++) {
      pixels.setPixelColor(i,green);
      pixels.show();
      delay(750);
    }
  } else if(bright<200) {
    for(int i=0; i<NUMPIXELS; i++) {
      pixels.setPixelColor(i,orange);
      pixels.show();
      delay(500);
    }
  } else if(bright<300) {
    for(int i=0; i<NUMPIXELS; i++) {
      pixels.setPixelColor(i,red);
      pixels.show();
      delay(100);
    }
  } else{
    pixels.setPixelColor(0,red);
    pixels.show();
    delay(50);
    
    pixels.setPixelColor(1,blue);
    pixels.show();
    delay(50);
    
    pixels.setPixelColor(2,orange);
    pixels.show();
    delay(50);
    
    pixels.setPixelColor(3,green);
    pixels.show();
    delay(50);
  }
  delay(100);
}