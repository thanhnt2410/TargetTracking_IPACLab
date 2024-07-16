#include <ESP8266WiFi.h>

// Thông tin WiFi
const char* ssid = "RSSI4";
const char* password = "11111111";

void setup() {
  Serial.begin(115200);
  //Serial2.begin(115200);
  connectWiFi();
}

void loop() {

  //byte rssi_send[1];
  int rssi = WiFi.RSSI();
  int rssi_send = abs(rssi);
  //rssi_send[0] = abs(rssi);
  //Serial.write(rssi_send,sizeof(rssi_send));
  Serial.println(String(rssi_send));
  //Serial.println();
}
void connectWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
