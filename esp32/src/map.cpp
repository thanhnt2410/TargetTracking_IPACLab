#include "map.h"

/*#########################################################################################################################*/
                                                    /*Draw map outline*/
/*#########################################################################################################################*/
void drawGrid(Adafruit_ILI9341& tft) 
{
  const int boxSize = 20;
  const int numBoxes = 10;
  const int yOrigin = 30;

  for (int x = 0; x <= numBoxes; x++) 
  {
    tft.drawLine(x * boxSize, yOrigin, x * boxSize, yOrigin + boxSize * numBoxes, ILI9341_WHITE);
  }
  for (int y = 0; y <= numBoxes; y++) 
  {
    tft.drawLine(0, yOrigin + y * boxSize, boxSize * numBoxes, yOrigin + y * boxSize, ILI9341_WHITE);
  }
}




/*#########################################################################################################################*/
                                                  /*Update user coordinate*/
/*#########################################################################################################################*/
void update_user_coordinate(Adafruit_ILI9341& tft, User user) 
{
  const int boxSize = 20;
  int screenX = (user.x - 1) * boxSize + 1 ;
  int screenY = (10 - user.y) * boxSize + 30 + 1; // Hệ tọa độ Decartes

  tft.fillRect(screenX, screenY, boxSize - 1, boxSize - 1, ILI9341_BLUE);
}





/*#########################################################################################################################*/
                                                /*Update fire coordinate*/
/*#########################################################################################################################*/
void update_fire_coordinate(Adafruit_ILI9341& tft, std::map<int, Fire> fires) 
{
  for (auto &fire : fires) 
  {
    if (fire.second.level > 0) 
    {
      uint16_t color;
      switch (fire.second.level) 
      {
        case 1: color = GREEN; break;
        case 2: color = YELLOW; break;
        case 3: color = ORANGE; break;
        case 4: color = RED; break;
        case 5: color = DARK_RED; break;
        default: color = ILI9341_BLACK; break;
      }
      const int boxSize = 20;
      int screenX = (fire.second.x - 1) * boxSize + 1 ;
      int screenY = (10 - fire.second.y) * boxSize + 30 + 1; // Hệ tọa độ Decartes
      
      tft.fillRect(screenX, screenY, boxSize - 1, boxSize - 1, color);
    }
  }
}





/*#########################################################################################################################*/
                                              /*Delete user/fire, old coordinate*/
/*#########################################################################################################################*/
void delete_old_user_coordinate(Adafruit_ILI9341& tft, User user) 
{
  const int boxSize = 20;
  int screenX = (user.x - 1) * boxSize + 1 ;
  int screenY = (10 - user.y) * boxSize + 30 + 1; // Hệ tọa độ Decartes

  tft.fillRect(screenX, screenY, boxSize - 1, boxSize - 1, ILI9341_BLACK);
}



void delete_old_fire_coordinate(Adafruit_ILI9341& tft, std::map<int, Fire> fires) 
{
  for (auto &fire : fires) 
  {
    if (fire.second.level > 0) 
    {
      const int boxSize = 20;
      int screenX = (fire.second.x - 1) * boxSize + 1 ;
      int screenY = (10 - fire.second.y) * boxSize + 30 + 1; // Hệ tọa độ Decartes

      tft.fillRect(screenX, screenY, boxSize - 1, boxSize - 1, ILI9341_BLACK);
    }
  }
}