#include <Wire.h>

#include "eduroamConnection.h"

void setup(){
    connectToWifi();

    Wire.begin();
    Serial.begin(115200);
    Serial.println("\nI2C Scanner");
}

void loop(){}