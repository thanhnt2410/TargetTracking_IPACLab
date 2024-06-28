#ifndef MAP_H
#define MAP_H





/*#########################################################################################################################*/
                                                       /*Define Lib*/
/*#########################################################################################################################*/

#include <Adafruit_ILI9341.h>
#include <map>
#include "main.h"
#include "colour.h"





/*#########################################################################################################################*/
                                                    /*Define Function*/
/*#########################################################################################################################*/

//Function draw map
void drawGrid(Adafruit_ILI9341& tft);

//Function draw new User/Fire coordinate
void update_user_coordinate(Adafruit_ILI9341& tft, User user);
void update_fire_coordinate(Adafruit_ILI9341& tft, std::map<int, Fire> fires);

//Function delete old User/Fire coordinate
void delete_old_user_coordinate(Adafruit_ILI9341& tft, User user);
void delete_old_fire_coordinate(Adafruit_ILI9341& tft, std::map<int, Fire> fires);





#endif
