#include "eduroamConnection.h"

#include <WiFi.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include <Wire.h>

#define SERVER_URL "https://test.mosquitto.org/"

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  delay(10);
  connectToWifi();
  client.setServer("test.mosquitto.org", 1883);
  Serial.println("\nI2C Scanner");
}

void loop() {
  // Connect the MQTT client
  if (!client.connected()){
    reconnect();
  }

  // Publish with the topic of cheese and the a custom cheesy message
  bool success = client.publish("cheese", "edam is the only cheese made backwards");
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
