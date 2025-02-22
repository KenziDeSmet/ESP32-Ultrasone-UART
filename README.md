# 📡 ESP32-Ultrasone-UART

This repo contains everything you need to read an ultrasonic distance sensor via UART and make the data available through a web server in JSON format. You'll find the code, an explanation of how it works, and a short guide on connecting the hardware.

Perfect for building an IoT project or just experimenting with sensors and web technology! 🚀

# 🛠️ Hardware

- ESP32 ([Amazon NL](https://www.amazon.nl/dp/B0CL9VZKMT?ref=ppx_yo2ov_dt_b_fed_asin_title))
- UART Ultrasone Sensor ([Amazon NL](https://www.amazon.nl/dp/B0DCVHBK3Q?ref=ppx_yo2ov_dt_b_fed_asin_title))
- Jumpers ([Amazon NL](https://www.amazon.nl/gp/product/B07DK31W19/ref=ewc_pr_img_1?smid=A3SCFTIO8CSK1X&psc=1))

# 💻 Software

- Arduino IDE ([Arduino Software](https://www.arduino.cc/en/software))
- CP210x USB to UART Bridge VCP Drivers ([Silabs Driver](https://www.silabs.com/developer-tools/usb-to-uart-bridge-vcp-drivers?tab=downloads))

# 🔗 Jumpers

GPIO Pins ESP32

```cpp
D16  -->  WHITE
3V3  -->  RED
GND  -->  BLACK
GND  -->  YELLOW
```

Pins on Sensor

```cpp
RED     -->  3V3
BLACK   -->  GND
YELLOW  -->  GND
WHITE   -->  D16
```

# ⚡ Quickstart

This code reads distance data via UART and displays it in the Serial Monitor.

```cpp
#include <SoftwareSerial.h>

SoftwareSerial mySerial(7, 8); // RX, TX

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
}

void loop() {
  if (mySerial.available()) {
    uint8_t data[4];
    for (int i = 0; i < 4; i++) {
      data[i] = mySerial.read();
    }

    if (data[0] == 0xFF) {
      double distance = (data[1] * 256 + data[2]);
      Serial.print(distance);
    }
  }
  delay(100);
}
```

# 📶 Wifi Connection

in `ESP32_Ultrasone-UART.ino` (lines 30 and 31), you'll need to change the SSID (name of your network) and the password to work with your own WiFi network:

```cpp
const char* ssid = "your_ssid";
const char* password = "your_password";
```

The `checkWiFiConnection()` function automatically reconnects to WiFi if the connection is lost. The LED indicates the status, and on success, the IP and MAC address are displayed.

```cpp
void checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(ledPin, LOW);
    Serial.println("WiFi connection lost!");
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print("Reconnecting to WiFi");
      WiFi.disconnect();
      WiFi.begin(ssid, password);
      int retryCount = 0;
      while (WiFi.status() != WL_CONNECTED && retryCount < 10) {
        delay(800);
        Serial.print(".");
        retryCount++;
      }
      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nReconnected to WiFi");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        Serial.print("MAC Address: ");
        Serial.println(WiFi.macAddress());
        break;
      }
      else {
        Serial.println("\nFailed to reconnect. Retrying...");
      }
    }
  }
  else {
    digitalWrite(ledPin, HIGH);
  }
}
```

# 🌐 Webserver

The `handleRoot()` function creates a JSON response with the measured distance and sends it to the client via a web server. When a client requests the root page, it receives a JSON object like:

```cpp
void handleRoot() {
    String jsonResponse = R"({"distance": )" + String(measuredDistance) + R"(})";
    server.send(200, "application/json", jsonResponse);
}
```
