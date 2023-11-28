#include "eduroamConnection.h"

#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include <math.h>

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

///////////////////////////// ESP32 TO ARDUINO COMMUNICATION ////////////////////////////////
// Send with 2 decimal places, so a temperature like 25.3 is mapped to 2530 (scaled by 100)
// 't' for temperature, 'p' for pH and 's' for stirring

void sendTemperature(float temperature){
  int temperatureData = (int)round(temperature * 100);
  sendDataToArduino('t', temperatureData);
}

void sendpH(float pH){
  int pHData = (int)round(pH * 100);
  sendDataToArduino('p', pHData);
}

void sendStirringRPM(float rpm){
  int rpmData = (int)round(rpm);
  sendDataToArduino('s', rpmData);
}

void sendDataToArduino(char subsystem, int data){
  Wire.beginTransmission(ESP_TO_NUCLEO_PORT);
  Wire.write(subsystem);
  Wire.write(highByte(data));
  Wire.write(lowByte(data));
  Wire.endTransmission();
  delay(50);
}

//////////////////////////////////////////////////////////////////////////////////////////////

float temperature = 27.34245;
float pH = 3.4942768523;
float rpm = 1532.356;

void loop() {
  sendTemperature(temperature);
  sendpH(pH);
  sendStirringRPM(rpm);

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
