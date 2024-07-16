#ifndef MAIN_H
#define MAIN_H





/*#########################################################################################################################*/
                                                       /*Define Lib*/
/*#########################################################################################################################*/

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>
#include <WiFi.h>
#include <cstdlib>
#include <map>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

// bool reconnect_MQTT();

// WiFiClientSecure espClient;
// PubSubClient client(espClient);


/*#########################################################################################################################*/ 
                                                /*Define Struct coordinate*/
/*#########################################################################################################################*/ 
struct User 
{
  int x;
  int y;
};

struct Fire 
{
  int x;
  int y;
  int level;
};
#endif