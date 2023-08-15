//include libraies (ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

//Wifi Connection Setting
#define WiFi_ID "USER_WiFi_ID"
#define WiFi_PW "USER_WiFi_PW"

ESP8266WebServer server(80);  // WebServer Object : Sets Server Port

int led = LED_BUILTIN;  //LED Built-In

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

void setup()  //The function that runs when the program starts
{
  pinMode(led,OUTPUT);
  digitalWrite(led,HIGH);
  Serial.begin(115200);
  Serial.println();
  WiFiSetup();  //Connects WiFi
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server operated.");
}

void handleRoot()  //The function that runs when the program starts
{
  digitalWrite(led,LOW);
  server.send(200,"text/plain","Hello from ESP8266");
  delay(100);
  digitalWrite(led,HIGH);
}

void loop()
{
  server.handleClient();
}


