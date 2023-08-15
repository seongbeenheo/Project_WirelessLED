//include libraies (ESP8266)
#include <ESP8266WiFi.h>

//Wifi Connection Setting
#define WiFi_ID "USER_WiFi_ID"
#define WiFi_PW "USER_WiFi_PW"

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
  Serial.begin(115200);
  Serial.println();
  WiFiSetup();  //Connects WiFi
}

void loop()  //The funciton that runs repeatedly
{
  Serial.println("working...");
  delay(10000);
}
