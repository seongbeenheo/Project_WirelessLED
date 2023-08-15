//include libraies (ESP8266, NeoPixel, Adafruit_IO, map)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>
#include "AdafruitIO_WiFi.h"
#include <map>

//Wifi Connection Setting
#define WiFi_ID "USER_WiFi_ID"
#define WiFi_PW "USER_WiFi_PW"

//Neopixel Setting
#define NUMPIXELS 4
#define PIXELPIN 4

//IO Server Setting
#define IO_USERNAME "USER_IO_USERNAME"
#define IO_KEY "USER_IO_KEY"

#if defined(USE_AIRLIFT) || defined(ADAFRUIT_METRO_M4_AIRLIFT_LITE) || \
    defined(ADAFRUIT_PYPORTAL)
#if !defined(SPIWIFI)
#define SPIWIFI SPI
#define SPIWIFI_SS 10  // Chip select pin
#define NINA_ACK 9     // a.k.a BUSY or READY pin
#define NINA_RESETN 6  // Reset pin
#define NINA_GPIO0 -1  // Not connected
#endif
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WiFi_ID, WiFi_PW, SPIWIFI_SS, NINA_ACK, NINA_RESETN, NINA_GPIO0, &SPIWIFI);
#else
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WiFi_ID, WiFi_PW);
#endif

//Adafruit IO Feeds Import (to get data from Adafruit.IO)
AdafruitIO_Feed *feed_color = io.feed("feed-color");
AdafruitIO_Feed *feed_bright = io.feed("feed-bright");

ESP8266WebServer server(80);  // WebServer Object : Sets Server Port

Adafruit_NeoPixel pixels(NUMPIXELS, PIXELPIN, NEO_GRB + NEO_KHZ800);  //NeoPixel Object, Please check the Neopixel Model ex> NEO_GRB

std::map<String, uint32_t> ColorMap = {  //ColorMap Dataframe
  {"OFF", pixels.Color(0, 0, 0)},
  {"RED", pixels.Color(255, 0, 0)},
  {"GREEN", pixels.Color(0, 255, 0)},
  {"BLUE", pixels.Color(0, 0, 255)},
  {"ORANGE", pixels.Color(255, 165, 0)},
  {"WHITE", pixels.Color(255, 255, 255)}
};

const char* ColorName[] = {"OFF", "RED", "GREEN", "BLUE", "ORANGE", "WHITE"};  //ColorName list
int numColorName = sizeof(ColorName) / sizeof(ColorName[0]);  //ColorName list length

//set Current ColorMode and Brightmode for global variables
String ColorMode = "OFF";
int BrightMode = 255;

String Color_Adafruit = "OFF";
int Bright_Adafruit = 255;

void WiFiSetup() {  //Connects WiFi
  WiFi.begin(WiFi_ID, WiFi_PW);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP Address: ");
  Serial.println(WiFi.localIP());
}

void IOSetup() {  //Connects to Adafruit IO
  Serial.print("Connecting to Adafruit IO");
  io.connect();
  feed_color->onMessage(handleColorAdafruit);
  feed_bright->onMessage(handleBrightnessAdafruit);
  while (io.status() != AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  feed_color->get();
  feed_bright->get();
  Serial.println();
  Serial.print("IO Status: ");
  Serial.println(io.statusText());
}

void HTTPSetup() {  //Sets HTTP Server
  server.on("/", handleROOT);
  server.on("/OFF", handleOFF);
  for (int i = 0; i < numColorName; i++)
  {
    String urllink = "/color/" + String(ColorName[i]);
    server.on(urllink, handleColorhttp);
  }
  for (int i = 0; i < 256; i++)
  {
    String urllink = "/brightness/" + String(i);
    server.on(urllink, handleBrightnesshttp);
  }
  server.onNotFound(handleonNotFound);
  server.begin();
  Serial.println("HTTP server successfully operated");
}

void PixelSetup() {  //Sets NeoPixel
  for (int i = 0; i < NUMPIXELS; i++)
  {
    pixels.setPixelColor(i, ColorMap["OFF"]);
  }
  pixels.show();
}

void setup()  //The function that runs when the program starts
{
  Serial.begin(115200);
  delay(5000);
  Serial.println();
  pixels.begin();
  WiFiSetup();  //Connects WiFi
  IOSetup();  //Connects to Adafruit IO
  HTTPSetup();  //Sets HTTP Server
  PixelSetup();  //Sets NeoPixel
  Serial.println("Current ColorMode: " + ColorMode);
  Serial.println("Current Brightness: " + BrightMode);
  Serial.println();
}

void loop()  //The funciton that runs repeatedly
{
  io.run();
  server.handleClient();
}

// LED settings
void setColor(String pixelcolor)
{
  for (int i = 0; i < NUMPIXELS; i++)
  {
    pixels.setPixelColor(i, ColorMap[pixelcolor]);
  }
  pixels.show();

  ColorMode = pixelcolor;
  Serial.println("Color changed.");
  Serial.println("Current ColorMode: " + String(ColorMode));
  Serial.println("Current Brightness: " + String(BrightMode));
}

void setBright(int pixelbright)
{
  pixels.setBrightness(pixelbright);
  pixels.show();

  BrightMode = pixelbright;

  Serial.println("Brightness changed.");
  Serial.println("Current ColorMode: " + String(ColorMode));
  Serial.println("Current Brightness: " + String(BrightMode));
}

// HTTP server
void handleROOT()
{
  String html = "<html>";
  html += "<head><title>ESP8266 Control Panel by S.B. HEO</title></head>";
  html += "<body>";
  html += "<h1>Welcome to SEONGBEEN HEO's ESP8266 Control Panel!!!!</h1>";
  html += "<h2>Current Color Mode : " + ColorMode + "</h2>";
  html += "<h2>Current Brightness : " + String(BrightMode) + "</h2>";
  html += "<h2>Set LED Color</h2>";
  html += "<ul>";
  for (int i = 0; i < numColorName; i++)
  {
    html += "<li><a href=\"/color/" + String(ColorName[i]) + "\">" + ColorName[i] + "</a></li>";
  }
  html += "</ul>";
  html += "<h2>Set LED Brightness</h2>";
  html += "<ul>";
  for (int i = 0; i < 256; i += 10)
  {
    html += "<li><a href=\"/brightness/" + String(i) + "\">" + String(i) + "</a></li>";
  }
  html += "</ul>";
  html += "</body>";
  html += "</html>";

  server.send(200, "text/html", html);
}

void handleonNotFound()
{
  server.send(200, "text/plain", "Invalid approach.");
}

void handleOFF()
{
  setColor("OFF");
  Serial.println("Operated by HTTP");
  Serial.println();
}

void handleColorhttp()
{
  String uri = server.uri();
  String color = uri.substring(7);
  Serial.println("Received color: " + color);
  if (ColorMap.count(color) > 0)
  {
    setColor(color);
    server.send(200, "text/plain", "Color set to: " + color);
    Serial.println("Operated by HTTP");
    Serial.println();
  }
  else
  {
    server.send(400, "text/plain", "Invalid color value");
  }
}

void handleBrightnesshttp()
{
  String url = server.uri();
  String brightnessValue = url.substring(12);
  int brightness = brightnessValue.toInt();
  Serial.println("Received brightness: " + brightnessValue);
  if (brightness >= 0 && brightness <= 255)
  {
    setBright(brightness);
    server.send(200, "text/plain", "Brightness set to: " + String(brightness));
    Serial.println("Operated by HTTP");
    Serial.println();
  }
  else
  {
    server.send(400, "text/plain", "Invalid brightness value");
  }
}

// Adafruit.io
void handleColorAdafruit(AdafruitIO_Data *data)
{
  Color_Adafruit = data->value();
  if (ColorMap.count(Color_Adafruit) > 0)
  {
    setColor(Color_Adafruit);
    Serial.println("Operate by Adafruit.io");
    Serial.println();
  }
}

void handleBrightnessAdafruit(AdafruitIO_Data *data)
{
  String Bright_Adafruit_String = data->value();
  Bright_Adafruit = Bright_Adafruit_String.toInt();
  if (Bright_Adafruit >= 0 && Bright_Adafruit <= 255)
  {
    setBright(Bright_Adafruit);
    Serial.println("Operate by Adafruit.io");
    Serial.println();
  }
}
