/*
 * Writed By Kenzi De Smet
 * Last Updated Thursday, 2 January 2025
 *
 * GPIO Pins ESP32
 *
 *   D16  -->  WHITE
 *   3V3  -->  RED
 *   GND  -->  BLACK
 *   GND  -->  YELLOW
 *
 * Pins on Sensor
 *
 *   RED     -->  3V3
 *   BLACK   -->  GND
 *   YELLOW  -->  GND
 *   WHITE   -->  D16
 *
 * Drivers
 *   CP210x USB to UART Bridge VCP Drivers
 *   https://www.silabs.com/developer-tools/usb-to-uart-bridge-vcp-drivers?tab=downloads
 *
 */

#include <WiFi.h>
#include <WebServer.h>

const int ledPin = 2;

const char* ssid = "your_ssid";
const char* password = "your_password";

WebServer server(80);
HardwareSerial mySerial(1);

volatile double measuredDistance = 0.0;
double calulatedDistance = 0.0;

void setup() {
    Serial.begin(9600);
    mySerial.begin(9600, SERIAL_8N1, 16, 17);
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);
    Serial.print("Connecting to WiFi ");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(800);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("MAC Address: ");
    Serial.println(WiFi.macAddress());
    do {
      readDistance();
      if (measuredDistance > 0) {
        server.on("/", handleRoot);
        server.begin();
        Serial.println("Web server started");
      }
    } while (measuredDistance == 0);
}

void loop() {
    server.handleClient();
    checkWiFiConnection();
    readDistance();
}

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

void handleRoot() {
    String jsonResponse = R"({"distance": )" + String(measuredDistance) + R"(, "percent": )" + String(calulatedDistance) + R"(})";
    server.send(200, "application/json", jsonResponse);
}

void readDistance() {
    if (mySerial.available() >= 4) {
        uint8_t data[4];
        for (int i = 0; i < 4; i++) {
            data[i] = mySerial.read();
        }
        if (data[0] == 0xFF) {
            measuredDistance = (data[1] * 256 + data[2]) / 10.0;
            if (( ( - (100.00 / 61.00) * measuredDistance) + (6500.00 / 61.00) ) >= 0) {
              calulatedDistance = ( ( - (100.00 / 61.00) * measuredDistance) + (6500.00 / 61.00) );
            }
            else {
              calulatedDistance = 0;
            }
        }
    }
}
