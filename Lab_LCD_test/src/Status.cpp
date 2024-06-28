/*#########################################################################################################################*/
                                                       /*Define Lib*/
/*#########################################################################################################################*/

#include "status.h"





/*#########################################################################################################################*/
                                            /*Function to draw outline/legend*/
/*#########################################################################################################################*/

void drawLegend(Adafruit_ILI9341& tft) 
{
    tft.drawLine(210, 25, 210, 239, WHITE);

    tft.fillRect(220, 40, 20, 20, BLUE);
    tft.setCursor(245, 50);
    tft.setTextColor(WHITE);
    tft.setTextSize(1);
    tft.println(":USER");

    tft.fillRect(220, 70, 4, 20, GREEN);
    tft.fillRect(224, 70, 4, 20, YELLOW);
    tft.fillRect(228, 70, 4, 20, ORANGE);
    tft.fillRect(232, 70, 4, 20, RED);
    tft.fillRect(236, 70, 4, 20, DARK_RED);
    tft.setCursor(245, 80);
    tft.setTextColor(WHITE);
    tft.setTextSize(1);
    tft.println(":FIRE (1->5)");

    tft.setCursor(220, 100);
    tft.setTextColor(WHITE);
    tft.setTextSize(1);
    tft.println("Status: ");
}





/*#########################################################################################################################*/
                                                /*Function to update sttus*/
/*#########################################################################################################################*/

void updateStatus(Adafruit_ILI9341& tft, User user, std::map<int, Fire> fires)
{
    tft.fillRect(265,100,50,10,BLACK);
    bool safe = true;
    tft.setCursor(265,100);
    tft.setTextSize(1);
    for (auto &fire : fires) 
    {
        if (fire.second.level > 0) 
        {
            if (abs(fire.second.x - user.x) <= 1 && abs(fire.second.y - user.y) <= 1) 
            {
                safe = false;
                break;
            } 
            else 
            {
                safe = true;
            }
        }
    }
    if(safe)
    {
        tft.setTextColor(GREEN);
        tft.println("SAFE");        
    }
    else
    {
        tft.setTextColor(RED);
        tft.println("DANGER");
    }
}