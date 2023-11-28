#include "eduroamConnection.h"

#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <PubSubClient.h>

#define SERVER_URL "https://test.mosquitto.org/"

const int ESP_TO_NUCLEO_PORT = 9;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);
  Wire.begin();
  delay(10);
  // connectToWifi();
  // client.setServer("test.mosquitto.org", 1883);
  // Wire.onReceive(sendToMQTT); // Register a function to be called when data is received
}

void loop() {
  Wire.beginTransmission(ESP_TO_NUCLEO_PORT);
  int value = 2500;
  Wire.write(highByte(value));
  Wire.write(lowByte(value));
  Wire.endTransmission();
  delay(1000);

  // Connect the MQTT client
  // if (!client.connected()){
  //   reconnect();
  // }

  // // Publish with the topic of cheese and the a custom cheesy message
  // bool success = client.publish("UCL_EE-CS_team21", "edam is the only cheese made backwards");
  // if (success){
  //   Serial.println("Message was published successfully!");
  // } else {
  //   Serial.println("There was a problem sending the message...");
  // }
  // delay(5000);
}

void printAvailableFromWire(){
  while (Wire.available()){
    char c = Wire.read();
    Serial.print(c);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void sendToMQTT(){
  ;
}
