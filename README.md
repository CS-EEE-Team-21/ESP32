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