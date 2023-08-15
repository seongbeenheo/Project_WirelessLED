//include libraies (ESP8266, NeoPixel, Adafruit_IO)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>
#include "AdafruitIO_WiFi.h"

//Wifi Connection Setting
#define WiFi_ID "USER_WiFi_ID"
#define WiFi_PW "USER_WiFi_PW""

//Neopixel Setting
#define NUMPIXELS 4
#define PIXELPIN 4

//IO Server Setting
#define IO_USERNAME "USER_IO_USERNAME"
#define IO_KEY "USER_IO_KEY"

#if defined(USE_AIRLIFT) || defined(ADAFRUIT_METRO_M4_AIRLIFT_LITE) ||         \
    defined(ADAFRUIT_PYPORTAL)
#if !defined(SPIWIFI_SS) 
#define SPIWIFI SPI
#define SPIWIFI_SS 10 // Chip select pin
#define NINA_ACK 9    // a.k.a BUSY or READY pin
#define NINA_RESETN 6 // Reset pin
#define NINA_GPIO0 -1 // Not connected
#endif
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WiFi_ID, WiFi_PW, SPIWIFI_SS,
                   NINA_ACK, NINA_RESETN, NINA_GPIO0, &SPIWIFI);
#else
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WiFi_ID, WiFi_PW);
#endif

//Adafruit IO Feeds Import (to get data from Adafruit.IO)
AdafruitIO_Feed *feed_color = io.feed("light");
AdafruitIO_Feed *feed_bright = io.feed("light-bright");

ESP8266WebServer server(80);  // WebServer Object : Sets Server Port

Adafruit_NeoPixel pixels(NUMPIXELS, PIXELPIN, NEO_GRB + NEO_KHZ800);  //NeoPixel Object, Please check the Neopixel Model ex> NEO_GRB

uint32_t RED = pixels.Color(255,0,0);  //ColorMap
uint32_t GREEN = pixels.Color(0,255,0);
uint32_t BLUE = pixels.Color(0,0,255);
uint32_t ORANGE = pixels.Color(255,165,0);
uint32_t WHITE = pixels.Color(255,255,255);
uint32_t OFF = pixels.Color(0,0,0);

String MODE = "OFF";  //ColorMode
String state;  //for Adafruit_IO

void WiFiSetup() {  //Connects WiFi
  WiFi.begin(WiFi_ID, WiFi_PW);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP Aderess: ");
  Serial.println(WiFi.localIP());
}

void IOSetup() {  //Connects to Adafruit IO
  Serial.print("Connecting to Adafruit IO");
  io.connect();
  feed_color->onMessage(handleMessageColor);
  feed_bright->onMessage(handleMessageBright);
  while (io.status() != AIO_CONNECTED) {
    Serial.print(F("."));
    delay(500);
  }

  feed_color->get();
  feed_bright->get();

  Serial.println();
  Serial.print("IO Status : ");
  Serial.println(io.statusText());
}

void HTTPSetup() {  //Sets HTTP Server
  server.on("/", handleROOT);
  server.on("/OFF", handleOFF);
  server.on("/RED", handleRED);
  server.on("/GREEN", handleGREEN);
  server.on("/BLUE", handleBLUE);
  server.on("/ORANGE", handleORANGE);
  server.on("/WHITE", handleWHITE);
  server.onNotFound(handleNO);

  server.begin();
  Serial.println("HTTP server successfully operated");
}

void PixelSetup() {  //Sets NeoPixel
  for(int i=0; i<NUMPIXELS; i++)
  {
    pixels.setPixelColor(i,OFF);
  }
  pixels.show();
}


void setup()  //The function that runs when the program starts
{
  Serial.begin(115200);
  Serial.println();
  pixels.begin();
  WiFiSetup();  //Connects WiFi
  IOSetup();  //Connects to Adafruit IO
  HTTPSetup();  //Sets HTTP Server
  Serial.print("Current MODE : ");
  Serial.println(MODE);
}

//HTTP Server
void handleROOT()
{
  server.send(200, "text/plain", "Hello from ESP8266.");
}

void handleOFF()
{

  for(int i=0; i<NUMPIXELS; i++)
  {
    pixels.setPixelColor(i,OFF);
  }
  pixels.show();
  server.send(200, "text/plain", "TURNED OFF");

  MODE = "OFF";
  Serial.print("Current MODE : ");
  Serial.println(MODE);
}

void handleRED()
{
  pixels.clear();

  for(int i=0; i<NUMPIXELS; i++)
  {
    pixels.setPixelColor(i,RED);
  }
  pixels.show();
  server.send(200, "text/plain", "SET as RED");

  MODE = "RED";
  Serial.print("Current MODE : ");
  Serial.println(MODE);
}

void handleGREEN()
{
  pixels.clear();

  for(int i=0; i<NUMPIXELS; i++)
  {
    pixels.setPixelColor(i,GREEN);
  }
  pixels.show();
  server.send(200, "text/plain", "SET as GREEN");

  MODE = "GREEN";
  Serial.print("Current MODE : ");
  Serial.println(MODE);
}

void handleBLUE()
{
  pixels.clear();

  for(int i=0; i<NUMPIXELS; i++)
  {
    pixels.setPixelColor(i,BLUE);
  }
  pixels.show();
  server.send(200, "text/plain", "SET as BLUE");

  MODE = "BLUE";
  Serial.print("Current MODE : ");
  Serial.println(MODE);
}

void handleORANGE()
{
  pixels.clear();

  for(int i=0; i<NUMPIXELS; i++)
  {
    pixels.setPixelColor(i,ORANGE);
  }
  pixels.show();
  server.send(200, "text/plain", "SET as ORANGE");

  MODE = "ORANGE";
  Serial.print("Current MODE : ");
  Serial.println(MODE);
}

void handleWHITE()
{
  pixels.clear();

  for(int i=0; i<NUMPIXELS; i++)
  {
    pixels.setPixelColor(i,WHITE);
  }
  pixels.show();
  server.send(200, "text/plain", "SET as WHITE");

  MODE = "WHITE";
  Serial.print("Current MODE : ");
  Serial.println(MODE);
}


void handleNO()
{
 server.send(200, "text/plain", "Invalid approach.");
}

void loop()
{
  io.run();
  server.handleClient();
}


//for Adafruit.io
void handleMessage(AdafruitIO_Data *data) {  
  state = data->value();
  if(state == "OFF")
  {
    handleOFF();
  }
  if(state == "RED")
  {
    handleRED();
  }
  if(state == "GREEN")
  {
    handleGREEN();
  }
  if(state == "BLUE")
  {
    handleBLUE();
  }
  if(state == "ORANGE")
  {
    handleORANGE();
  }
  if(state == "WHITE")
  {
    handleWHITE();
  }
}