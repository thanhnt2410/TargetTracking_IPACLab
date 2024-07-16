#include <WiFi.h>

// Thông tin WiFi
const char* ssid = "HP";
const char* password = "3141592654";

void setup() {
  Serial.begin(115200);
  //Serial2.begin(115200);
  connectWiFi();
}

void loop() {

  byte rssi_send[1];
  int rssi = WiFi.RSSI();

  rssi_send[0] = abs(rssi);
  Serial.write(rssi_send,sizeof(rssi_send));
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
