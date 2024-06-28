/*#########################################################################################################################*/
                                                    /*include function*/
/*#########################################################################################################################*/
//Function
#include "main.h"
#include "colour.h"
#include "position.h"
#include "map.h"
#include "status.h"
#include "button.h"





/*#########################################################################################################################*/
                                              /*define output SPI, loop time*/
/*#########################################################################################################################*/                  
#define TFT_CS    15
#define TFT_RST   4
#define TFT_DC    2
#define TFT_MOSI  23
#define TFT_CLK   18
#define TFT_MISO  19
#define TFT_LED   21
#define BUTTON_PIN 13

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

unsigned long lastUpdate = 0;
#define UPDATE_INTERVAL 500  // Time loop (100ms)





/*########################################################################################################################*/
                                                    /*define coordinate*/
/*########################################################################################################################*/

// Biến lưu tọa độ user
User user_new_coordinate = {0, 0};
User user_old_coordinate = {0, 0};



// Map lưu trữ thông tin fire
std::map<int, Fire> fire_new_coordinates;
std::map<int, Fire> fire_old_coordinates;





/*########################################################################################################################*/
                                                /*Set up - Intro - Outline*/
/*########################################################################################################################*/

void setup() 
{
  //Set up Start
  Serial.begin(9600);  // Khởi động Serial monitor

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH);  // Bật đèn nền

  tft.begin();
  tft.setRotation(1);  // Xoay màn hình ngang



  //Intro
  tft.fillScreen(WHITE);
  tft.setCursor(40, 50);
  tft.setTextColor(RED);
  tft.setTextSize(5);
  tft.println("iPac Lab");

  tft.setCursor(30, 150);
  tft.setTextColor(BLACK);
  tft.setTextSize(3);
  tft.println("Target Tracking");

  delay(5000);



  //outline

  //iPAC Lab, Target tracking
  tft.fillScreen(BLACK);
  tft.setCursor(115, 5);
  tft.setTextColor(RED);
  tft.setTextSize(2);
  tft.println("iPAC Lab");
  tft.setCursor(30, 150);
  tft.setTextColor(BLACK);
  tft.setTextSize(3);
  tft.println("Target Tracking");

  //Draw map
  drawGrid(tft);

  //Draw Legend
  drawLegend(tft);

  //Connect to wifi - mqtt
  setupWifi();
  setupMqtt();

  //
  buttonSetup(BUTTON_PIN);
}





/*########################################################################################################################*/
                                                            /*Loop*/
/*########################################################################################################################*/

void loop() 
{
  //Check connect to MQTT
  loopMqtt();
  
  //Delay
  unsigned long currentMillis = millis();
  if (currentMillis - lastUpdate >= UPDATE_INTERVAL) 
  {
    lastUpdate = currentMillis;

    //Take the Old coordinate
    user_old_coordinate = user_new_coordinate;
    fire_old_coordinates = fire_new_coordinates;

    //Update position of user/fire
    getUserPosition(user_new_coordinate);
    getFireLocation(fire_new_coordinates);

    //Delete old User/Fire
    delete_old_user_coordinate(tft, user_old_coordinate);
    delete_old_fire_coordinate(tft, fire_old_coordinates);
    //Draw User/Fire on map
    update_user_coordinate(tft, user_new_coordinate);
    update_fire_coordinate(tft, fire_new_coordinates);
    
    //Update Status
    updateStatus(tft, user_new_coordinate, fire_new_coordinates);

    //countdown
    for (auto &f : fire_new_coordinates) 
    {
      if (f.second.level > 0 && abs(f.second.x - user_new_coordinate.x) <= 1 && abs(f.second.y - user_new_coordinate.y) <= 1) 
      {
        drawRectangles(tft, f.second.level);
        handleButtonPress(tft, user_new_coordinate , fire_new_coordinates, BUTTON_PIN);
      }
    }
  //
  }
  
}