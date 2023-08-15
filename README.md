# Project_WirelessLED
This is the code of Wireless LED Control Server using ESP8266 Chipset-Based Board and Neopixel LED.

It is based on Arduino IDE, using c++


I used such modules.
1ST :  <ESP8266WiFi.h>, <ESP8266WebServer.h> ▶▶ to operate HTTPServer

2ND :  "AdafruitIO_WiFi.h" ▶▶ to connect to Adafruit.IO ( I didn't use Adafruit_MQTT because of Unstable Connection. )

3RD :  <Adafruit_NeoPixel.h> ▶▶ to operate HTTPServer

LAST : <map> ▶▶ to use DataFrame on C++


The Default Unit Setting of Brightness and Custom RGB Color is 10 (from 0 to 250 by 10).

I wanna set Unit Setting to 1, but The board said "Out of Memory...".

If you have good-performace Board, You can try to set Unit Setting to 1.


Made this code on C.A. in DongYang High School, located in Seoul, South Korea.
