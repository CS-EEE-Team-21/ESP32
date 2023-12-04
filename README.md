# ESP32

## Setting up

In order to the eduroam wifi network, you will have to provide the eduroamConnection.ino file with your credentials.
To do so, create a credentials.h file in the project's root and include the following code:

```
// credentials.h
#ifndef CREDENTIALS_H
#define CREDENTIALS_H

const char* ssid = "your ucl connection email address";
const char* password = "your password";

#endif
```

## MQTT Broker
For testing purposes, there is a publically avialable server at https://test.mosquitto.org/ where we can test the sending and receiving of requests. So far, I have been able to get text from the ESP32 to be successfully published to the public Mosquitto server. What needs to be worked on now is having our ESP32 subscribe to any topics sent from the broker. That, as well as actually hooking up the MQTT data to our frontend.

Here is an example for the protocol of how sensor data should be sent, and how it should look within the code.

```
#include <PubSubClient.h>
```
Add this library from within the Arduino IDE (the one by Nick O'Leary. Other details such as initialising the PubSubClient and connecting can be found in [this video.](https://www.youtube.com/watch?v=x5A5S0hoyJ0)

```
bool success = client.publish("temperature", our_temperature_reading_in_celsius);
```
This publishes the temperature reading under the topic of "temperature". The `success` value returned tells you if the message was successfully published.

## Initial MQTT Testing
You can monitor the MQTT Broker on the command line on Windows in order to test if the ESP32 is indeed publishing the message. All that is needed is to have some other device "subscribe" to the same topic, in order to receive the message. In these examples. we are still using the public testing server.

1. Install Moquitto
2. Go to the directory that Mosquitto was installed in and open up command prompt with admin privileges
3. Run the command `net start mosquitto`

I followed [this tutorial](https://www.youtube.com/watch?v=4ZEPPQLY5o4) to set up the Mosquitto service on my laptop.

To **subscribe** to the temperature reading, run this command in the same directory as Mosquitto:
```
mosquitto_sub -t temperature -h test.mosquitto.org
```

To **publish** a temperature reading (for when we need to test if the ESP32 can receive data):
```
mosquitto_pub -t temperature -h test.mosquitto.org -m "Your temperature here"
```

## I<sup>2</sup>C Connections
To get this to work, you must connect both your ESP32 and Nucleoboard to a laptop, so that you can compile the code for both microcontrollers at the same time. You will need three wires to connect the microcontrollers together: for the SDA, SCL and also one to connect the ground pins together. In our case, here are the connections we used:

| ESP32 | Nucleo Board    |
|-------|-----------------|
| 22    | SCL             |
| 23    | SDA             |
| GND   | GND (POWER)     |

## Wire.h Library
Using the ESP32 as the master device, and the Nucleoboard as the slave device, we implemented the example functions showcased [here](https://docs.arduino.cc/learn/communication/wire#wire-library) and created a protocol of our own. Given that the devices were connected physically, then there was no need to mess up the file structure by nesting folders within each other unnecessarily. All that was needed was to choose any valid port number for the slave device, and have the ESP32 try to initiate contact with that port. 

* If we want to update our dashboard with the current sensor readings, then the ESP32 calls a function `Wire.requestfrom()` to request the readings from the Nucleoboard.
* If we specify a new target temperature/pH/stirring speed from the dashboard, then the ESP32 uses `Wire.write()` to pass on the new values to the Nucleoboard.

## I<sup>2</sup>C Protocol
Since data had to be read from Wire a single byte at a time, it was easiest to make sure that all the data being sent was in the form of a fixed-width integer, like `uint16_t`. It was far easier to deal with than sending floats by the byte, so that meant converting a temperature reading like `26.32` into an int of some kind. This is what was came up with, where all the integers being sent through were rounded to the nearest integer.

* Temperature: **Multiply by 100** to send a reading like `26.32` as `2632`.
* pH: **Multiply by 100** to send a reading like `5.37` as `537`.
* Stirring speed: **Round to the nearest integer** to send a reading like `1893.5` as `1894`.

From ESP32 to NucleoBoard, it's important to be able to differentiate between the values from different subsystems, so this is indicated by the characters, 't', 'p' and 's'. For instance, the message that would be sent from the ESP32 to indicate that the target temperature should change to 26.5°C would be something along the lines of `t 2650`.

From NucleoBoard to ESP32, it is less important to have separate functions to fetch the values from each sensor, since all the data is being put into the database anyway. Therefore, the function `getReadings()` is used to get the values of every sensor in a single go.



