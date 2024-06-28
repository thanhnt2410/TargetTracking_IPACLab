#include "button.h"
#include <Arduino.h>
#include <map>
#include "Adafruit_ILI9341.h"

#define BUTTON_PIN 13
#define DEBOUNCE_DELAY 50 // Thời gian chống dội tính bằng milliseconds

bool isButtonPressed = false;
bool lastButtonState = HIGH;   // Trạng thái trước đó của nút (kéo lên, HIGH mặc định)
bool buttonState = HIGH;       // Trạng thái hiện tại của nút
unsigned long lastDebounceTime = 0;  // Thời gian lần cuối cùng thay đổi trạng thái
unsigned long buttonPressTime = 0;   // Thời gian bắt đầu giữ nút
unsigned long lastCountdownTime = 0; // Thời gian lần cuối cùng đếm ngược
bool isCountingDown = false;   // Trạng thái đang đếm ngược
int countdown = 0; // Giá trị hiện tại của đếm ngược
int currentFireKey = -1;
uint16_t colors[] = {ILI9341_GREEN, ILI9341_YELLOW, ILI9341_ORANGE, ILI9341_RED, ILI9341_MAROON};

void buttonSetup(int pin) {
    pinMode(pin, INPUT_PULLUP);
}

void drawRectangles(Adafruit_ILI9341& tft, int count) 
{
    tft.fillRect(235, 200, 95, 40, BLACK);
    int heights[] = {10, 15, 20, 25, 30};
    int x = 300; // Vị trí x của các hình chữ nhật (gần góc dưới bên phải)
    int y = 230; // Vị trí y bắt đầu vẽ (dưới cùng)

    for (int i = 0; i < count; ++i) 
    {
        tft.fillRect(x - (10 + 5) * i, y - heights[i], 10, heights[i], colors[i]);
    }
}

void handleButtonPress(Adafruit_ILI9341& tft, User user, std::map<int, Fire>& fire, int pin)
{
    int reading = digitalRead(pin);
    if (reading != lastButtonState) 
    {
        lastDebounceTime = millis(); // Đặt lại thời gian chờ khi có thay đổi
    }
    if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY)
    {
        if (reading != buttonState)
        {
            buttonState = reading;
            if (buttonState == LOW)
            {
                for (auto &f : fire) 
                {
                    if (f.second.level > 0 && abs(f.second.x - user.x) <= 1 && abs(f.second.y - user.y) <= 1) 
                    {
                        countdown = f.second.level;
                        currentFireKey = f.first;
                        Serial.print("Button ON\n");
                        Serial.print(countdown);
                        isCountingDown = true;
                        buttonPressTime = millis();
                        lastCountdownTime = buttonPressTime;
                        break; // Dừng lại sau khi tìm thấy ngọn lửa gần nhất
                    }
                }
            }
            else
            {
                Serial.println("Button OFF");
                isCountingDown = false;
            }
        }
    }
    if (isCountingDown) {
        unsigned long currentMillis = millis();
        if (currentMillis - lastCountdownTime >= 1000) 
        {
            countdown--;
            lastCountdownTime = currentMillis;

            if (countdown > 0) 
            {
                tft.fillRect(220, 220, 11, 13, ILI9341_BLACK);
                tft.setCursor(220, 223);
                tft.setTextColor(ILI9341_GREEN);
                tft.setTextSize(1);
                tft.print(countdown);
                tft.println("s");
                tft.fillRect(240, 230, 100, 300, ILI9341_BLACK);
                Serial.println(countdown);
                fire[currentFireKey].level = countdown; // Giảm level của ngọn lửa đi 1 mỗi giây
                update_fire_coordinate(tft, fire);
                drawRectangles(tft, countdown);
            } 
            else 
            {
                isCountingDown = false;
                tft.fillRect(220, 220, 11, 13, ILI9341_BLACK);
                tft.setCursor(220, 223);
                tft.setTextColor(ILI9341_GREEN);
                tft.setTextSize(1);
                tft.println("OK");
                Serial.println("Countdown finished");
            }
        }
    }
    lastButtonState = reading;
}
