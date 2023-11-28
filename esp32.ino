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

///////////////////////////// ESP32 TO NUCLEOBOARD COMMUNICATION //////////////////////////////////
// Send with 2 decimal places, so a temperature like 25.3 is mapped to 2530 (scaled by 100)
// 't' for temperature, 'p' for pH and 's' for stirring

void sendTemperature(float temperature){
  uint16_t temperatureData = (int)round(temperature * 100);
  sendDataToArduino('t', temperatureData);
}

void sendpH(float pH){
  uint16_t pHData = (int)round(pH * 100);
  sendDataToArduino('p', pHData);
}

void sendStirringRPM(float rpm){
  uint16_t rpmData = (int)round(rpm);
  sendDataToArduino('s', rpmData);
}

void sendDataToArduino(char subsystem, uint16_t data){
  Wire.beginTransmission(ESP_TO_NUCLEO_PORT);
  Wire.write(subsystem);
  Wire.write(highByte(data));
  Wire.write(lowByte(data));
  Wire.endTransmission();
  delay(50);
}

///////////////////////// ESP32 FROM NUCLEOBOARD COMMUNICATION //////////////////////////////////

// In the order: temperature, pH, stirring
void getReadings(){
  Wire.requestFrom(ESP_TO_NUCLEO_PORT, 6);
  delay(50);
  byte bytes[6];

  for (int i=0; i<6; i++){
    bytes[i] = Wire.read();
  }

  uint16_t temperature = word(bytes[0], bytes[1]);
  uint16_t pH = word(bytes[2], bytes[3]);
  uint16_t rpm = word(bytes[4], bytes[5]);

  processReceivedTemperature(temperature);
  processReceivedpH(pH);
  processReceivedStirringRPM(rpm);
}

void processReceivedTemperature(float temperature){
  temperature /= 100;
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println("°C");
}

void processReceivedpH(float pH){
  pH /= 100;
  Serial.print("pH: ");
  Serial.println(pH);
}

void processReceivedStirringRPM(int rpm){
  Serial.print("Motor RPM: ");
  Serial.println(rpm);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);
  Wire.begin();
  // delay(10);
  // connectToWifi();
  // client.setServer("test.mosquitto.org", 1883);
  // Wire.onReceive(sendToMQTT); // Register a function to be called when data is received
}

void loop() {
  getReadings();
  delay(1000);
  // sendTemperature(31.42);
  // sendpH(3.2945);
  // sendStirringRPM(2023);

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
