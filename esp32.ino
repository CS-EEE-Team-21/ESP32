#include "eduroamConnection.h"
#include "I2C.h"

#include <WiFi.h>
#include <HTTPClient.h>
#include <PubSubClient.h>

#define SERVER_URL "https://test.mosquitto.org/"

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  delay(10);
  connectToWifi();
  client.setServer("test.mosquitto.org", 1883);

  // Setting up I2C connection to Nucleo board
  Wire.begin(8); // Join the bus with address #8
  Wire.onReceive(sendToMQTT); // Register a function to be called when data is received
  Serial.begin(9600);
}

void loop() {
  // Connect the MQTT client
  if (!client.connected()){
    reconnect();
  }

  // Publish with the topic of cheese and the a custom cheesy message
  bool success = client.publish("UCL_EE-CS_team21", "edam is the only cheese made backwards");
  if (success){
    Serial.println("Message was published successfully!");
  } else {
    Serial.println("There was a problem sending the message...");
  }
  delay(5000);
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
  while (1 < Wire.available()) { // Loop through all received data
    char c = Wire.read(); // Receive byte
    Serial.print(c); // Print byte
  }
  int x = Wire.read(); // Receive last byte
  Serial.println(x); // Print last byte
}
