/*#########################################################################################################################*/
                                                      /*Define Lib*/
/*#########################################################################################################################*/

#include "position.h"





/*#########################################################################################################################*/
                                              /*Wifi - MQTT info / topic sub*/
/*#########################################################################################################################*/

unsigned long lastReconnectAttempt = 0;

//ssid - Pasword Wifi
const char* ssid = ".";
const char* password = "........";

// MQTT broker
const char* mqtt_server = "192.168.39.45";  //IP address of computer run Mosquitto broker
const char* topic_user = "user";              //topic user
const char* topic_fire = "fire";              //topic fire

//Set up connect
WiFiClient espClient;
PubSubClient client(espClient);

//defaut coordinate
std::string user_string = "x1 y1";
std::string fire_string = "f1 x0 y0 l0, f2 x0 y0 l0, f3 x0 y0 l0, f4 x0 y0 l0, f5 x0 y0 l0";

/*#########################################################################################################################*/
                                    /*Check Wifi - MQTT connect / reconnect / loopMQTT*/
/*#########################################################################################################################*/

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
  lastReconnectAttempt = 0;
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
