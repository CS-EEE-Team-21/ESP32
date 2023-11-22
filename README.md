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



