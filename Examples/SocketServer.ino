#include "ESP8266WiFi.h"
 
const char* ssid = "NA";
const char* password = "116028E44B";
 
WiFiServer wifiServer(11000);

void setup() {
 
  Serial.begin(115200);
 
  delay(1000);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting..");
  }
 
  Serial.print("Connected to WiFi. IP:");
  Serial.println(WiFi.localIP());
 
  wifiServer.begin();
}

void loop() {
 
  WiFiClient client = wifiServer.available();
  Serial.println("No Clients Available..");
  delay(1000);
  if (client) {
 
    while (client.connected()) {
 
      while (client.available()>0) {
        char c = client.read();
        Serial.write(c);
        client.write(c);
      }
 
      delay(10);
    }
 
    client.stop();
    Serial.println("Client disconnected");
 
  }
  else
   Serial.println("No Clients Available..");
}
