#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "VNPT-T4";
const char* password = "017016068";
#define MQTT_SERVER "192.168.82.102"
const int  MQTT_PORT = 1883;
#define MQTT_USER "iPAC_Lab"
#define MQTT_PASSWORD "12345678"
#define MQTT_TOPIC "REST"

const int potentiometerValve = 33;
const int switchPin = 13;

// Check I2C device address and correct line below (by default address is 0x29 or 0x28)
//                                   id, address
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);

WiFiClient wifiClient;
PubSubClient client(wifiClient);
void setup_wifi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void connect_to_broker() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "IoTLab";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("connected");

      client.subscribe(MQTT_TOPIC);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      delay(2000);
    }
  }
}

void setup(void)
{
  Serial.begin(115200);
  while (!Serial) delay(10);  // wait for serial port to open!
  /* Initialise the sensor */
  if (!bno.begin())
  {
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }
  setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT );
  connect_to_broker();

  pinMode(potentiometerValve, INPUT);
  pinMode(switchPin, INPUT_PULLUP);
}

void loop(void)
{
  if (!client.connected()) {
  connect_to_broker();
  }
  client.loop();
  double x = -1000000, y = -1000000 , z = -1000000, x1 = -1000000, y1 = -1000000 , z1 = -1000000;
  int valveState = -1000, switchState = -1000;
  sensors_event_t accelerometerData, orientationData;
  
  bno.getEvent(&accelerometerData, Adafruit_BNO055::VECTOR_ACCELEROMETER);
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);

  x = accelerometerData.acceleration.x;
  y = accelerometerData.acceleration.y;
  z = accelerometerData.acceleration.z;

  x1 = orientationData.orientation.x;
  y1 = orientationData.orientation.y;
  z1 = orientationData.orientation.z;

  int potValue = analogRead(potentiometerValve);
  valveState = map(potValue, 0, 4096, 0, 100);

  bool currentswitchState = digitalRead(switchPin) == LOW;
  if(currentswitchState == true)  switchState = 1;
  if(currentswitchState == false) switchState = 0;

  // Tạo JSON payload
  StaticJsonDocument<300> doc;
  doc["switchState"] = switchState;
  doc["valveState"] = valveState;
  doc["aX"] = x;
  doc["aY"] = y;
  doc["aZ"] = z;
  doc["oX"] = x1;
  doc["oY"] = y1;
  doc["oZ"] = z1;

  char buffer[512];
  size_t n = serializeJson(doc, buffer);

  // Xuất bản thông điệp JSON lên MQTT broker
  client.publish(MQTT_TOPIC, buffer, n);

  delay(200);
}

