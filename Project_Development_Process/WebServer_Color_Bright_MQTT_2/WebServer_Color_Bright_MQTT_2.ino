//include libraies (ESP8266, Neopixel, Adafruit IO, MQTT, map)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>
#include "AdafruitIO_WiFi.h"
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <map>

//Wifi Connection Setting
#define WiFi_ID "USER_WiFi_ID"
#define WiFi_PW "USER_WiFi_PW"

//Neopixel Setting
#define NUMPIXELS 4
#define PIXELPIN 4

//MQTT Server Setting
#define IO_SERVER "io.adafruit.com"
#define IO_SERVERPORT 1883
#define IO_USERNAME "USER_IO_USERNAME"
#define IO_KEY "USER_IO_KEY"

const unsigned long MQTTSetupInterval = 15 * 1000; //Interval Setting for MQTTSetup() : miliseconds

#if defined(USE_AIRLIFT) || defined(ADAFRUIT_METRO_M4_AIRLIFT_LITE) || defined(ADAFRUIT_PYPORTAL)
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

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, IO_SERVER, IO_SERVERPORT, IO_USERNAME, IO_KEY);

//Adafruit IO Feeds Import (to get data from Adafruit.IO)
AdafruitIO_Feed *feed_color = io.feed("feed-color");
AdafruitIO_Feed *feed_bright = io.feed("feed-bright");
AdafruitIO_Feed *feed_mqtt = io.feed("feed-mqtt");

//Adafruit IO Feeds Export (to send data to Adafruit.IO)
Adafruit_MQTT_Publish feed_color_out = Adafruit_MQTT_Publish(&mqtt, IO_USERNAME  "/feeds/feed-color-out");
Adafruit_MQTT_Publish feed_bright_out = Adafruit_MQTT_Publish(&mqtt, IO_USERNAME  "/feeds/feed-bright-out");

ESP8266WebServer server(80);  // WebServer Object : Set Server Port

Adafruit_NeoPixel pixels(NUMPIXELS, PIXELPIN, NEO_GRB + NEO_KHZ800);  //NeoPixel Object, Please check the Neopixel Model ex> NEO_GRB

std::map<String, uint32_t> ColorMap = {  //ColorMap Dataframe 
  {"OFF", pixels.Color(0, 0, 0)},
  {"RED", pixels.Color(255, 0, 0)},
  {"GREEN", pixels.Color(0, 255, 0)},
  {"BLUE", pixels.Color(0, 0, 255)},
  {"ORANGE", pixels.Color(255, 165, 0)},
  {"WHITE", pixels.Color(255, 255, 255)}
};

const char* ColorName[] = {"OFF", "RED", "GREEN", "BLUE", "ORANGE", "WHITE"}; //ColorName list
const int numColorName = sizeof(ColorName) / sizeof(ColorName[0]); //ColorName list length 

//set Current ColorMode and Brightmode for global variables
String ColorMode = "OFF";
int BrightMode = 255;
unsigned long lastMQTTSetupTime = 0; // set the time when MQTTSetup is called recently. 


void WiFiSetup(){  //Connects WiFi
  WiFi.begin(WiFi_ID, WiFi_PW);

  Serial.print(F("Connecting"));
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print(F("Connected, IP Address: "));
  Serial.println(WiFi.localIP());
}

void IOSetup() {  //Connects to Adafruit IO
  Serial.print(F("Connecting to Adafruit IO"));
  io.connect();
  while (io.mqttStatus() < AIO_CONNECTED) {
    Serial.print(F("."));
    delay(500);
  }

  feed_color->onMessage(handleColorAdafruit);
  feed_bright->onMessage(handleBrightAdafruit);
  feed_mqtt->onMessage(handlemqttAdafruit);
  feed_color->get();
  feed_bright->get();
  feed_mqtt->get();
  Serial.println();
  Serial.print(F("IO Status: "));
  Serial.println(io.statusText());
}

void MQTTSetup() {  //Connects to MQTT
  mqtt.connect();
  Serial.print(F("Connecting to MQTT"));
  while (!mqtt.connected()) {
    Serial.print(F("."));
    delay(100);
  }
  Serial.println();
  Serial.println(F("Connected to MQTT"));
  Serial.println();
}

void HTTPSetup() {  //Sets HTTP Server
  server.on("/", handleROOT); 
  server.on("/OFF", handleOFF);
  for (int i = 0; i < numColorName; i++) {  
    String urllink = "/color/" + String(ColorName[i]);
    server.on(urllink, handleColorhttp);
  }
  for (int i = 0; i < 256; i++) {
    String urllink = "/brightness/" + String(i);
    server.on(urllink, handleBrighthttp);
  }
  server.on("/reboot",handleReboot);
  server.on("/MQTTconnect",handleMQTTConnect);
  server.onNotFound(handleonNotFound);
  server.begin();
  Serial.println(F("HTTP server successfully operated"));
}

void PixelSetup() {  //Sets NeoPixel
  pixels.begin();
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, ColorMap["OFF"]);
  }
  pixels.show();
}

void CurrentSerial() {  //Shows Current ColorMode and Brightness to Serial
  Serial.print(F("Current ColorMode: "));
  Serial.println(ColorMode);
  Serial.print(F("Current Brightness: "));
  Serial.println(String(BrightMode));
}

void MQTTsend() {  //Publishes (sends) data to MQTT
  if (!mqtt.connected()) {
    Serial.print("MQTT Disconnected. ");
    MQTTSetup();
  }
  feed_color_out.publish(ColorMode.c_str());
  feed_bright_out.publish(BrightMode);
}

void setup() {  //The function that runs when the program starts
  Serial.begin(115200);
  delay(5000);
  Serial.println();
  WiFiSetup();  //Connect WiFi
  IOSetup();  //Connect to Adafruit IO
  MQTTSetup();  //Connect to MQTT
  HTTPSetup();  //Set HTTP Server
  PixelSetup();  //Set NeoPixel
  CurrentSerial();  //Show Current ColorMode and Brightness to Serial
  Serial.println();
}

void loop()  //The funciton that runs repeatedly
{
  unsigned long currentTime = millis();  //Connect MQTT in Interval
  io.run();
  if (currentTime - lastMQTTSetupTime >= MQTTSetupInterval) {
    lastMQTTSetupTime = currentTime;
    Serial.print(F("Auto "));
    MQTTSetup();
    MQTTsend();
  }
  if (!mqtt.connected()) {  //Auto Reconnect MQTT when disconnected
    Serial.print("MQTT Disconnected. ");
    MQTTSetup();
  }
  if (io.mqttStatus() < AIO_CONNECTED) { //Auto Reconnect Adafruit IO when disconnected
    Serial.print("Adafruit IO Disconnected. ");
    IOSetup(); 
  }
  server.handleClient(); // Handle HTTP Requests ( It helps to turn on the server)
}



// NeoPixel LED Settings ( Btightness and Color )
void setColor(String pixelcolor) {  //Sets 
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, ColorMap[pixelcolor]);
  }
  pixels.show();
  ColorMode = pixelcolor;
  Serial.println("Color changed.");
  CurrentSerial();
}

void setBright(int pixelbright) {
  pixels.setBrightness(pixelbright);
  pixels.show();
  BrightMode = pixelbright;
  Serial.println("Brightness changed.");
  CurrentSerial();
}


// HTTP server
void handleROOT() {  //Main Page on HTTP WebServer. Controls Brightness, Color, System settings(Reboot, MQTT connect)
  String htmlRoot = "<html>";
  htmlRoot += "<head><meta http-equiv='refresh' content='10'><title>ESP8266 Control Panel by S.B. HEO</title>";
  htmlRoot += "<script>";
  htmlRoot += "function setBrighthtml(value) {";    //setBrighthtml() javascript function
  htmlRoot += "  var xhr = new XMLHttpRequest();";
  htmlRoot += "  xhr.open('GET','/brightness/' + value, true);";
  htmlRoot += "  xhr.send();";
  htmlRoot += "}";
  htmlRoot += "function setColorhtml(color) {";     //seCcolorhtml() javascript function
  htmlRoot += "  var xhr = new XMLHttpRequest();";
  htmlRoot += "  xhr.open('GET','/color/' + color, true);";
  htmlRoot += "  xhr.send();";
  htmlRoot += "}";
  htmlRoot += "</script>";
  htmlRoot += "</head><body>";
  htmlRoot += "<h1>Welcome to SEONGBEEN HEO's ESP8266 Control Panel!!!!</h1>";
  htmlRoot += "<h2>This site is automatically refreshed every 15 seconds.</h2>";
  htmlRoot += "<h2></h2>";
  htmlRoot += "<h2>Current Color Mode : " + ColorMode + "</h2>";
  htmlRoot += "<h2>Current Brightness : " + String(BrightMode) + "</h2>";
  htmlRoot += "<h2>Set LED Color</h2>";
  htmlRoot += "<ul>";
  for (int i = 0; i < numColorName; i++) {
    htmlRoot += "<li><a href=\"javascript:setColorhtml('" + String(ColorName[i]) + "')\">" + ColorName[i] + "</a></li>";
  }
  htmlRoot += "</ul>";
  htmlRoot += "<h2>Set LED Brightness</h2>";
  htmlRoot += "<ul>";
  htmlRoot += "<input type='range' min='0' max='255' value='" + String(BrightMode) + "' onchange='setBrighthtml(this.value)'>";
  htmlRoot += "<span>" + String(BrightMode) + "</span>";
  htmlRoot += "</ul>";
  htmlRoot += "<h2></h2>";
  htmlRoot += "<h2><a href=\"/reboot\">REBOOT!!</a></h2>";
   htmlRoot += "<h2><a href=\"/MQTTconnect\">MQTT reconnect!!</a></h2>";
  htmlRoot += "</body>";
  htmlRoot += "</html>";
  server.send(200, "text/html", htmlRoot);
  MQTTsend();
}

void handleonNotFound() {  //Shows when user tries to access a page that does not exist on HTTP WebServer
  String htmlonNotFound = "<html>";
  htmlonNotFound += "<head><title>Invalid approach.</title>";
  htmlonNotFound += "</head><body>";
  htmlonNotFound += "<h1>Invalid approach.</h1>";
  htmlonNotFound += "</body>";
  htmlonNotFound += "</html>";
  server.send(404, "text/html", htmlonNotFound);
}
void handleReboot() {  //Reboots the ESP8266 for developers on HTTP WebServer
  String htmlReboot = "<html>";
  htmlReboot += "<head><meta http-equiv='refresh' content='3;url=/'><title>Rebooting...</title>";
  htmlReboot += "</head><body>";
  htmlReboot += "<h1>Rebooting...</h1>";
  htmlReboot += "<p>Redirecting to main site in 3 seconds...</p>";
  htmlReboot += "</body>";
  htmlReboot += "</html>";
  server.send(200, "text/html", htmlReboot);
  Serial.println(F("Rebooting..."));
  ESP.restart(); //reboot ESP8266
}

void handleMQTTConnect() {  //Reconnects MQTT server to Adafruit IO on HTTP WebServer
  String htmlMQTTConnect = "<html>";
  htmlMQTTConnect += "<head><meta http-equiv='refresh' content='15;url=/'><title>Reconnecting MQTT...</title>";
  htmlMQTTConnect += "</head><body>";
  htmlMQTTConnect += "<h1>Reconnecting MQTT...</h1>";
  htmlMQTTConnect += "<h2><a href=\"/\">Go Back to Main Site</a></h2>";
  htmlMQTTConnect += "</body>";
  htmlMQTTConnect += "</html>";
  server.send(200, "text/html", htmlMQTTConnect);
  Serial.println(F("Reconnecting MQTT..."));
  MQTTSetup();
  MQTTsend();
}

//html for handleColorhttp and handleBrighthttp on HTTP WebServer
void htmlSetting(String option, String optionmode) {  //only String ex> color, RED / brightness , 254
  String htmlset = "<html>";
  htmlset += "<head><meta http-equiv='refresh' content='3;url=/'><title>ESP8266 "+ option + " Set : " + optionmode + "</title>";
  htmlset += "</head><body>";
  htmlset += "<h1>" + option + " Set to :" + optionmode + "!</h1>";
  htmlset += "<h2></h2>";
  htmlset += "<li><a href=\"/\">Go Back to Main Site</a></li>";
  htmlset += "</body>";
  htmlset += "</html>";
  server.send(200, "text/html", htmlset);
}

void htmlInvaild(String option) {  //only String ex> color / brightness
  String htmlInvaild = "<html>";
  htmlInvaild += "<head><meta http-equiv='refresh' content='3;url=/'><title>Invalid " + option + " Value.</title>";
  htmlInvaild += "</head><body>";
  htmlInvaild += "<h1>Invalid " + option + " Value.</h1>";
  htmlInvaild += "<h2></h2>";
  htmlInvaild +=  "<li><a href=\"/\">Go Back to Main Site</a></li>";
  htmlInvaild += "</body>";
  htmlInvaild += "</html>";
  server.send(400, "text/html", htmlInvaild);
}

void handleOFF() {  //Turns off NeoPixel LED on HTTP WebServer, not ESP8266!!!
  setColor("OFF");
  Serial.println(F("Operated by HTTP"));
  Serial.println();
  String htmlOFF = "<html>";
  htmlOFF += "<head><meta http-equiv='refresh' content='15;url=/'><title>ESP8266 Turned OFF</title>";
  htmlOFF += "</head><body>";
  htmlOFF += "<h1>ESP8266 Turned OFF!</h1>";
  htmlOFF += "<h2></h2>";
  htmlOFF += "<li><a href=\"/\">Go Back to Main Site</a></li>";
  htmlOFF += "</body>";
  htmlOFF += "</html>";
  server.send(200, "text/html", htmlOFF);
  MQTTsend();
}

void handleColorhttp() {  //Controls the color of NeoPixel LED on HTTP WebServer
  String uri = server.uri();
  String color = uri.substring(7);
  String setting = "color";
  Serial.print(F("Received color: "));
  Serial.println(color);
  if (ColorMap.count(color) > 0) {
    setColor(color);
    htmlSetting(setting, ColorMode);
    Serial.println(F("Operated by HTTP"));
    Serial.println();
  } else {
    htmlInvaild(setting);
  }
  MQTTsend();
}

void handleBrighthttp() {  //Controls the Brightness of NeoPixel LED on HTTP WebServer
  String url = server.uri();
  String bright_string = url.substring(12); //string
  int bright = bright_string.toInt();  //int
  String setting = "brightness";
  Serial.print(F("Received brightness: "));
  Serial.println(bright_string);
  if (bright >= 0 && bright <= 255) {
    setBright(bright);
    htmlSetting(setting, String(BrightMode));
    Serial.println(F("Operated by HTTP"));
    Serial.println();
  } else {
    htmlInvaild(setting);
  }
  MQTTsend();
}
  

// Adafruit.io
void handleColorAdafruit(AdafruitIO_Data *data) {  //Controls the Color of NeoPixel LED on Adafruit.IO
  if (ColorMap.count(ColorMode) > 0) {
    setColor(ColorMode);
    Serial.println(F("Operated by Adafruit.io"));
    Serial.println();
  }
  MQTTsend();
}

void handleBrightAdafruit(AdafruitIO_Data *data) {  //Controls the Brightness of NeoPixel LED on Adafruit.IO
  String Bright_Adafruit_String = data->value();
  BrightMode = Bright_Adafruit_String.toInt();
  if (BrightMode >= 0 && BrightMode <= 255) {
    setBright(BrightMode);
    Serial.println(F("Operated by Adafruit.io"));
    Serial.println();
  }
  MQTTsend();
}

void handlemqttAdafruit(AdafruitIO_Data *data) {  //Reconnects MQTT on Adafruit.IO
  String mqtt_value = data->value();
  if(mqtt_value == "MQTT") {
    MQTTSetup();
    MQTTsend();
    Serial.println(F("Operated by Adafruit.io"));
    Serial.println();
  }
}