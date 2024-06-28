#ifndef BUTTON_H
#define BUTTON_H

#include <map>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "colour.h"
#include "main.h"
#include "map.h"

void buttonSetup(int pin);
void drawRectangles(Adafruit_ILI9341& tft, int count);
void handleButtonPress(Adafruit_ILI9341& tft, User user , std::map<int, Fire>& fire ,int pin);

#endif
