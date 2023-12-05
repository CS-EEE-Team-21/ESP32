#include "eduroamConnection.h"

#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include <math.h>
#include <string>

#define SERVER_URL "https://test.mosquitto.org/"

const int ESP_TO_NUCLEO_PORT = 9;

WiFiClient espClient;
PubSubClient client(espClient);

///////////////////////////// ESP32 TO NUCLEOBOARD COMMUNICATION //////////////////////////////////
// Send with 2 decimal places, so a temperature like 25.3 is mapped to 2530 (scaled by 100)
// 't' for temperature, 'p' for pH and 's' for stirring

void updateTargetTemperature(float temperature){
  uint16_t temperatureData = (int)round(temperature * 100);
  Serial.println(temperatureData);
  sendDataToArduino('t', temperatureData);
}

void updateTargetPH(float pH){
  uint16_t pHData = (int)round(pH * 100);
  sendDataToArduino('p', pHData);
}

void updateTargetRotation(float rpm){
  uint16_t rpmData = (int)round(rpm);
  sendDataToArduino('s', rpmData);
}

void sendDataToArduino(char subsystem, uint16_t data){
  Wire.beginTransmission(ESP_TO_NUCLEO_PORT);
  Wire.write(subsystem);
  Wire.write(highByte(data));
  Wire.write(lowByte(data));
  Wire.endTransmission();
}

///////////////////////////////////// ESP32 FROM NUCLEOBOARD COMMUNICATION //////////////////////////////////

// In the order: temperature, pH, stirring
void getReadings(){
  Wire.requestFrom(ESP_TO_NUCLEO_PORT, 6);
  delay(50);

  if (Wire.available() != 0){
    byte bytes[6];
  
    for (int i=0; i<6; i++){
      bytes[i] = Wire.read();
    }
  
    uint16_t temperature = word(bytes[0], bytes[1]);
    uint16_t pH = word(bytes[2], bytes[3]);
    uint16_t rpm = word(bytes[4], bytes[5]);
    
    if (temperature != 0) processReceivedTemperature(temperature);
    if (pH != 0) processReceivedpH(pH);
    if (rpm != 0) processReceivedStirringRPM(rpm);
  } else {
    Serial.println("Nucleoboard needs to be recompiled!");
    delay(3000);
  }
}

void processReceivedTemperature(float temperature){
  temperature /= 100;
  sendToMQTT("temp:" + String(temperature));
}

void processReceivedpH(float pH){
  pH /= 100;
  sendToMQTT("ph:" + String(pH));
}

void processReceivedStirringRPM(int rpm){
  sendToMQTT("rots:" + String(rpm));
}

////////////////////////////////////// ESP32 TO MQTT CONNECTION ///////////////////////////////////////

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

// Receive the messages in the form "ph:4.5" for example, and parse to change target values
// 'rots' = rotation, 'temp' = temperature, 'ph' = pH
void processTargetValues(char *input) {
    char *colonPosition = strchr(input, ':');

    if (colonPosition != nullptr) {
      // Separate subsystem name and value
      size_t subsystemLength = colonPosition - input;
      size_t dataLength = strlen(colonPosition + 1);
      char *subsystem = new char[subsystemLength + 1];
      char *data = new char[dataLength + 1];
      strncpy(subsystem, input, subsystemLength);
      subsystem[subsystemLength] = '\0';
      strncpy(data, colonPosition + 1, dataLength);
      data[dataLength] = '\0';

      // Send parsed values to the Nucleoboard to update target values
      float numericData = std::strtof(data, nullptr);
      String subsystemString = String(subsystem);

      if (subsystemString == "rots") {
        updateTargetRotation(numericData);
      } else if (subsystemString == "ph") {
        updateTargetPH(numericData);
      } else if (subsystemString == "temp") {
        updateTargetTemperature(numericData);  
      } else {
        Serial.print("Error, unknown subsystem ");
        Serial.println(subsystemString);
      }

      delete[] subsystem;
      delete[] data;
    } else {
      Serial.println("Colon not found in the input string");
    }
}

void sendToMQTT(String message){
  Serial.println(message);

  // Sending the readings to the Mosquitto server
  bool pubSuccess = client.publish("UCL_EE-CS_team21", message.c_str());
  if (!pubSuccess){
    Serial.println("There was a problem sending the message...");
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message: ");
  char message[length+1];
  for (int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';
  Serial.println(message);
  processTargetValues(message);
}

//////////////////////////////////////// LOOP AND SETUP FUNCTIONS////////////////////////////////////////

void setup() {
  Serial.begin(9600);
  Wire.begin();
  delay(10);
  connectToWifi();
  client.setServer("test.mosquitto.org", 1883);
  client.setCallback(callback);
}

void loop() {
  getReadings();
  // updateTargetTemperature(31.41);
  // updateTargetPH(3.2945);
  // updateTargetRotation(2023);

  // Connect the MQTT client
  if (!client.connected()){
    reconnect();
  }

  // Reading data
  bool subSuccess = client.subscribe("UCL_EE-CS_team21_targets");
  if (!subSuccess){
    Serial.println("There was a problem receiving the message...");
  }

  client.loop();
}