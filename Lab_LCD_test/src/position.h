#ifndef POSITION_H
#define POSITION_H





/*#########################################################################################################################*/
                                                       /*Define Lib*/
/*#########################################################################################################################*/

#include <map>
#include <WiFi.h>
#include <PubSubClient.h>
#include <string>
#include <sstream>
#include "main.h"






/*#########################################################################################################################*/
                                                    /*Define Function*/
/*#########################################################################################################################*/

//Function get user position
void getUserPosition(User& user_coordinate);

//Function get fire location
void getFireLocation(std::map<int, Fire>& fire_coordinates);

//Function connect Wifi, Mosquitto MQTT
void setupWifi();
void setupMqtt();
void loopMqtt();
void callback(char* topic, byte* payload, unsigned int length);





#endif
