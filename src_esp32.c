
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
// // #include <WiFiClient.h>
#include "main.h"
#include "colour.h"
#include "map.h"
#include "status.h"
#include <map>
#include <WiFi.h>
#include <PubSubClient.h>
#include <string>
#include <sstream>
#include "main.h"

// //----Thay đổi thành thông tin của bạn---------------

const char* ssid = "IPAC_LAB_Member";
const char* password = "12345678";
const char* mqtt_server = "192.168.8.105";
const int  MQTT_PORT = 1883;
#define MQTT_USER "iPAC_Lab"
#define MQTT_PASSWORD "12345678"
#define MQTT_TOPIC0 "REST"
#define MQTT_TOPIC1 "RSSI"
const char* topic_user = "user";              //topic user
const char* topic_fire = "fire";              //topic fire
//--------------------------------------------------
WiFiClient wifiClient;
PubSubClient client(wifiClient);
unsigned long lastReconnectAttempt = 0;
unsigned long timeUpdata=millis();

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];


#define TIMEOUT 1000
// Định nghĩa các chân kết nối
#define TFT_CS    2
#define TFT_RST   4
#define TFT_DC    5
#define TFT_MOSI  23
#define TFT_CLK   18
#define TFT_MISO  19
//định nghĩa các chân ngoại vi
#define BT1       25          
#define BT2       26      // chân này ko hoạt động
#define BT3       27
#define BT4       17
#define bientro1  32
#define bientro2  33
#define pinA      13      //chan pinA mux
#define pinB      14      //chan pinB mux

#define MQTT_TOPIC0 "REST"
#define MQTT_TOPIC1 "RSSI"


Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
/* Set the delay between fresh samples */
uint16_t BNO055_SAMPLERATE_DELAY_MS = 100;

// Check I2C device address and correct line below (by default address is 0x29 or 0x28)
//                                   id, address
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);

int rssi1 ,rssi2 ,rssi3 ,rssi4 ;
int val_bt1, val_bt2, val_bt3, val_bt4, val_bientro1, val_bientro2;
unsigned long lastUpdate = 0;
unsigned long time_rc_data =0;
#define UPDATE_INTERVAL 500  // Thời gian cập nhật màn hình


Khai báo các hàm chức năng
void rc_data();     //Hàm nhận data từ esp07
void tft_setup();   //hàm test tft
void bno_run();
void bno_begin();
void printEvent(sensors_event_t* event);
void ReadBT();      //đọc giá trị biến trở và nút nhấn
void sendDataToMQTT();
void publishMessage(const char* topic, String payload, boolean retained);
void display_tft();
void selectESP8266(int esp_num);
String readFromESP();
//void connect_to_broker();
bool reconnect();


// /*########################################################################################################################*/
//                                                     /*define coordinate*/
// /*########################################################################################################################*/

// Biến lưu tọa độ user
User user_new_coordinate = {0, 0};
User user_old_coordinate = {0, 0};



// Map lưu trữ thông tin fire
std::map<int, Fire> fire_new_coordinates;
std::map<int, Fire> fire_old_coordinates;

std::string user_string = "x1 y1";
std::string fire_string = "f1 x0 y0 l0, f2 x0 y0 l0, f3 x0 y0 l0, f4 x0 y0 l0, f5 x0 y0 l0";
//Function get user position
void getUserPosition(User& user_coordinate);

//Function get fire location
void getFireLocation(std::map<int, Fire>& fire_coordinates);

//Function connect Wifi, Mosquitto MQTT
void setupWifi();
void setupMqtt();
void loopMqtt();
void callback(char* topic, byte* payload, unsigned int length);


//---------------Khai bao gia tri bien-------------------
int valve_bientro1 , val_button1 , valve_bientro2 ;
double OrientX , OrientY, OrientZ ; // dumb values, easy to spot problem
double LinearX , LinearY , LinearZ ;
// /*
void setup() 
{
  Serial.begin(115200);  // Khởi động Serial monitor
  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);
  pinMode(BT1, INPUT);
  pinMode(BT2, INPUT);
  pinMode(BT3, INPUT);
  pinMode(BT4, INPUT);
  pinMode(bientro1, INPUT);
  pinMode(bientro2, INPUT);
  Serial.println("iPAC Lab");
  setupWifi();

  tft.begin();
  tft.setRotation(1);  // Xoay màn hình ngang
  tft.begin();
  tft_setup();
  bno_begin();

    Draw map
  drawGrid(tft);

  Draw Legend
  drawLegend(tft);
  tft.setCursor(220, 220);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(1);
  tft.println("connect MQTT");
  client.setServer(MQTT_SERVER, MQTT_PORT);
  setupMqtt();

  // Khởi tạo và vẽ bản đồ
}

void loop() 
{
  loopMqtt();
  tft.fillScreen(ILI9341_BLACK);
  rc_data();
  client.loop();
  bno_run();
  sendDataToMQTT();
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
  }
  display_tft();

}


// //------------Khai báo các hàm ngoại vi -----------------------------
void rc_data()
{
  
  tft.setCursor(0, 30);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(2);
  String data1 , data2 , data3, data4;
  //int rssi1_rc = 95, rssi2_rc = 95, rssi3_rc = 95, rssi4_rc = 95;
  int rssi[4];
  for (int i = 0; i < 4; i++) {
        selectESP8266(i);
        delay(10);
        Serial.end();
        delay(10);
        Serial.begin(115200);

        String data = readFromESP();
        rssi[i] = data.toInt();
        time_rc_data =millis();
        while(rssi[i]<10)
        {
          String data = readFromESP();
          rssi[i] = data.toInt();

        }
        // tft.setCursor(220, 130);
        // tft.println("RSSI " + String(i+1) + ": " + String(rssi[i]));
        // if (data.length() > 0) {
          //rssi[i] = data.toInt();
          
        // } else {
    
        // }
        delay(10);
    }
            rssi1 = rssi[0];
            rssi2 =rssi[1];
            rssi3 = rssi[2];
            rssi4 = rssi[3];
}

// // // --------------Hàm TFT ---------------------
void tft_setup(){
  tft.setRotation(1);  // Xoay màn hình ngang
  // Hiển thị chữ "iPac Lab" và "Target Tracking" trên khung nền màu trắng
  tft.fillScreen(ILI9341_WHITE);
  tft.setCursor(40, 50);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(5);
  tft.println("iPac Lab");

  tft.setCursor(30, 150);
  tft.setTextColor(ILI9341_BLACK);
  tft.setTextSize(3);
  tft.println("Target Tracking");

  delay(5000);  // Hiển thị thông báo trong 5 giây

  // Đổi khung nền sang màu đen và hiển thị chữ "iPAC Lab"
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(115, 5);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(2);
  tft.println("iPAC Lab");
}
void display_tft()
{
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(1);
    tft.setCursor(220, 150);
    tft.print("RSSI1: " +String(rssi1));
    tft.setCursor(220, 170);
    tft.print("RSSI2: " + String(rssi2));
    tft.setCursor(220, 190);
    tft.print("RSSI3: " + String(rssi3));
    tft.setCursor(220, 210);
    tft.print("RSSI4: " +String(rssi4));

}

void bno_begin(){
  if (!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }

  delay(1000);
}
void bno_run(){
//could add VECTOR_ACCELEROMETER, VECTOR_MAGNETOMETER,VECTOR_GRAVITY...
  sensors_event_t orientationData , angVelocityData , linearAccelData, magnetometerData, accelerometerData, gravityData;
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);
  printEvent(&orientationData);
  printEvent(&linearAccelData);
  uint8_t system, gyro, accel, mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);

  delay(BNO055_SAMPLERATE_DELAY_MS);
}

void printEvent(sensors_event_t *event)
{
  // double OrientX = -1000000, OrientY = -1000000, OrientZ = -1000000; // dumb values, easy to spot problem
  // double LinearX = -1000000, LinearY = -1000000, LinearZ = -1000000;

  if (event->type == SENSOR_TYPE_ORIENTATION)
  {
    //Serial.print("Orient:");
    OrientX = event->orientation.x;
    OrientY = event->orientation.y;
    OrientZ = event->orientation.z;

  }
  else if (event->type == SENSOR_TYPE_LINEAR_ACCELERATION)
  {
    //Serial.print("Linear:");
    LinearX = event->acceleration.x;
    LinearY = event->acceleration.y;
    LinearZ = event->acceleration.z;

  }

  else
  {
    // Serial.print("Unk:");
  }
}
void ReadBT()
{
  val_bt1 = digitalRead(BT1);
  val_bt2 = digitalRead(BT2);
  val_bt3 = digitalRead(BT3);
  val_bientro1 = analogRead(bientro1);
  val_bientro2 = analogRead(bientro2);
}

// // //---------MQTT-------------------
// // //-----Method for Publishing MQTT Messages---------
void publishMessage(const char* topic, String payload, boolean retained){
  if(client.publish(topic,payload.c_str(),true))
    Serial.println("Message published ["+String(topic)+"]: "+payload);
}



// //#########################################################################################################################
//                                               //Push string to struct user/fire
// //#########################################################################################################################

// // Function get user position
void getUserPosition(User& user_coordinate) 
{
    sscanf(user_string.c_str(), "x%d y%d", &user_coordinate.x, &user_coordinate.y);
    Serial.println(String(user_coordinate.x)+", " +String(user_coordinate.y));
    // sscanf(user_string.c_str(), "x%d y%d", 5, 5);
}



// // Function get fire location
void getFireLocation(std::map<int, Fire>& fire_coordinates) 
{
  fire_coordinates.clear();
  std::stringstream ss(fire_string);
  std::string token;
  while (std::getline(ss, token, ',')) 
  {
    int x, y, level;
    char key[3];
    sscanf(token.c_str(), "%s x%d y%d l%d", key, &x, &y, &level);
    int fire_id = key[1] - '0';  // Convert char to int
    fire_coordinates[fire_id] = {x, y, level};
  }
  
}

void sendDataToMQTT()
{
  // Tạo JSON payload
  JsonDocument information;
  information["nutnhan"]["button1"] = val_button1;
  information["bientro"]["bientro1"] = valve_bientro1;
  information["bientro"]["bientro2"] = valve_bientro2;
  information["bno055"]["Linear"]["LinearX"] = LinearX;
  information["bno055"]["Linear"]["LinearY"] = LinearY;
  information["bno055"]["Linear"]["LinearZ"] = LinearZ;
  information["bno055"]["Orient"]["OrientX"] = OrientX;
  information["bno055"]["Orient"]["OrientY"] = OrientY;
  information["bno055"]["Orient"]["OrientZ"] = OrientZ;

  char buffer[512];
  size_t n = serializeJson(information, buffer);

  // Xuất bản thông điệp JSON lên MQTT broker
  client.publish(MQTT_TOPIC0, buffer, n);

  // Tạo JSON payload
  JsonDocument rssi;
  rssi["RSSI"]["rssi1"] = rssi1;
  rssi["RSSI"]["rssi2"] = rssi2;
  rssi["RSSI"]["rssi3"] = rssi3;
  rssi["RSSI"]["rssi4"] = rssi4;

  char buffer1[512];
  size_t n1 = serializeJson(rssi, buffer1);

  // Xuất bản thông điệp JSON lên MQTT broker
  client.publish(MQTT_TOPIC1, buffer1, n1);
  delay(200);
}
// 
// //-------------------ESP8266 SETUP------------------------

void selectESP8266(int esp_num) {
    switch (esp_num) {
        case 0:
            digitalWrite(pinA, LOW);
            digitalWrite(pinB, LOW);
            break;
        case 1:
            digitalWrite(pinA, HIGH);
            digitalWrite(pinB, LOW);
            break;
        case 2:
            digitalWrite(pinA, LOW);
            digitalWrite(pinB, HIGH);
            break;
        case 3:
            digitalWrite(pinA, HIGH);
            digitalWrite(pinB, HIGH);
            break;
    }
    delay(50); // Chờ cho mux ổn định
}
String readFromESP() {
    unsigned long startTime = millis();
    String data = "";
    while ((millis() - startTime) < TIMEOUT) {
        if (Serial.available() > 0) {
            data = Serial.readStringUntil('\n');
            break;
        }
    }
    return data;
}



/*#########################################################################################################################*/
                                                    /*include function*/
/*#########################################################################################################################*/
//Function


unsigned long lastReconnectAttempt = 0;

//ssid - Pasword Wifi
const char* ssid = "IPAC_LAB_Member";
const char* password = "12345678";
const char* mqtt_server = "192.168.8.102";
const char* topic_user = "user";              //topic user
const char* topic_fire = "fire";              //topic fire

//Set up connect
WiFiClient espClient;
PubSubClient client(espClient);

//defaut coordinate
std::string user_string = "x1 y1";
std::string fire_string = "f1 x0 y0 l0, f2 x0 y0 l0, f3 x0 y0 l0, f4 x0 y0 l0, f5 x0 y0 l0";

//Function get user position
void getUserPosition(User& user_coordinate);

//Function get fire location
void getFireLocation(std::map<int, Fire>& fire_coordinates);

//Function connect Wifi, Mosquitto MQTT
void setupWifi();
void setupMqtt();
void loopMqtt();
void callback(char* topic, byte* payload, unsigned int length);

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
                                                /*Set up - Intro - Outline*/
/*########################################################################################################################*/
void setupWifi() 
{
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  int max_attempts = 20;
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < max_attempts) 
  {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("");
    Serial.println("Failed to connect to WiFi");
    Serial.print("WiFi status code: ");
    Serial.println(WiFi.status());
  }
}



bool reconnect() 
{
  if (client.connect("ESP32Client")) {
    Serial.println("connected");
    client.subscribe(topic_user);
    Serial.print("Subscribed to topic: ");
    Serial.println(topic_user);
    client.subscribe(topic_fire);
    Serial.print("Subscribed to topic: ");
    Serial.println(topic_fire);
    return true;
  } else {
    Serial.print("failed, rc=");
    Serial.print(client.state());
    Serial.println(" try again in 10 seconds");
    return false;
  }
}



void setupMqtt() 
{
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
}



void loopMqtt() 
{
  if (!client.connected()) 
  {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > 5000)  // Time to reconnect 5s
    {
      lastReconnectAttempt = now;
      if (reconnect()) 
      {
        lastReconnectAttempt = 0;
      }
    }
  } 
  else 
  {
    client.loop();
  }
}





/*#########################################################################################################################*/
                                                  /*read msg, callback*/
/*#########################################################################################################################*/

void callback(char* topic, byte* payload, unsigned int length) 
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String message;
  for (unsigned int i = 0; i < length; i++) 
  {
    Serial.print((char)payload[i]);
    message += (char)payload[i];
  }
  Serial.println();
  if (String(topic) == "user") 
  {
    user_string = message.c_str();
  } 
  else if (String(topic) == "fire") 
  {
    fire_string = message.c_str();
  }
}

/*#########################################################################################################################*/
                                              /*Push string to struct user/fire*/
/*#########################################################################################################################*/

// Function get user position
void getUserPosition(User& user_coordinate) 
{
    sscanf(user_string.c_str(), "x%d y%d", &user_coordinate.x, &user_coordinate.y);
}



// Function get fire location
void getFireLocation(std::map<int, Fire>& fire_coordinates) 
{
  fire_coordinates.clear();
  std::stringstream ss(fire_string);
  std::string token;
  while (std::getline(ss, token, ',')) 
  {
    int x, y, level;
    char key[3];
    sscanf(token.c_str(), "%s x%d y%d l%d", key, &x, &y, &level);
    int fire_id = key[1] - '0';  // Convert char to int
    fire_coordinates[fire_id] = {x, y, level};
  }
}
//--------------------------------------------------------------------
