#include <ESP8266WiFi.h>
#include <SimpleKalmanFilter.h>
// Thông tin WiFi
const char* ssid = "RSSI3";
const char* password = "11111111";
SimpleKalmanFilter loc_nhieu(5,5,1);

void setup() {
  Serial.begin(115200);
  connectWiFi();
}

void loop() {
  byte rssi_send[1];
  // rssi_send[0] = 0x00;
  // rssi_send[2] = 0xff;
  int rssi = WiFi.RSSI();
  int rssi_filter = loc_nhieu.updateEstimate(rssi);
  rssi_send[0] = abs(rssi_filter);
  //Serial.println(rssi_send[0]);
  Serial.write(rssi_send,sizeof(rssi_send));
  delay(10); // In dữ liệu RSSI mỗi 10 milgiây
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