#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "VNPT-T4";
const char* password = "017016068";
#define MQTT_SERVER "192.168.82.101"
const int  MQTT_PORT = 1883;
#define MQTT_USER "iPAC_Lab"
#define MQTT_PASSWORD "12345678"
#define MQTT_TOPIC "RSSI"

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
  setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT );
  connect_to_broker();

}
double randomDouble() {
    return (double)rand() / RAND_MAX;
}
void loop(void)
{
  if (!client.connected()) {
  connect_to_broker();
  }
  client.loop();
  double x = -1000000, y = -1000000 , z = -1000000, x1 = -1000000;
  x = randomDouble();
  y = randomDouble();
  z = randomDouble();
  x1 = randomDouble();
  // Tạo JSON payload
  StaticJsonDocument<200> doc;
  doc["RSSI1"] = x;
  doc["RSSI2"] = y;
  doc["RSSI3"] = z;
  doc["RSSI4"] = x1;

  char buffer[512];
  size_t n = serializeJson(doc, buffer);

  // Xuất bản thông điệp JSON lên MQTT broker
  client.publish(MQTT_TOPIC, buffer, n);

  delay(200);
}