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

## Connectivity

In order to create HTTP requests to a web server, you must include the HTTPClient.h header file.
The following code below is an example of how you can fetch the page content from the ESP32. I will do some research into POST requests so that we can work towards making requests to the ReactJS server.

```
#include <HTTPClient.h>
#define serverUrl "https://example.com/"
```

The rest of the eduroam connection code in `setup()` goes here...

```
void loop() {
  // Create an HTTP object
  HTTPClient http;

  // Specify the server and resource
  http.begin(serverUrl);

  // Send the GET request
  int httpResponseCode = http.GET();

  // Check for a successful response
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    // Get the content of the response
    String payload = http.getString();
    Serial.println("Response content:");
    Serial.println(payload);
  } else {
    Serial.print("HTTP Request failed. Error code: ");
    Serial.println(httpResponseCode);
  }

  // Close the connection
  http.end();

  // Wait for a moment before sending the next request
  delay(5000);
}
```
