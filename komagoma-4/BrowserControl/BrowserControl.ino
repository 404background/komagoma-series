#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include "controller.h"

const int ServoPin1 = D1;
const int ServoPin2 = D2;
const int DutyMax = 2300;
const int DutyMin = 700;
int speed1 = 0;
int speed2 = 0;

const char *ssid = "komagoma-4";
const char *password = "123456789";
WiFiServer server(80);

void ServoSpeed(int pin, int speed) {     //speed -10~10
  if (speed != 0) {
    int Duty = map(speed, -10, 10, DutyMin, DutyMax);
    digitalWrite(pin, HIGH);
    delayMicroseconds(Duty);
    digitalWrite(pin, LOW);
    delayMicroseconds(20 * 1000 - Duty);
  }
}

void setup() {
  pinMode(ServoPin1, OUTPUT);
  pinMode(ServoPin2, OUTPUT);
  ServoSpeed(ServoPin1, 5);
  ServoSpeed(ServoPin2, 5);

  while (!WiFi.softAP(ssid, password)) {}
  IPAddress myIP = WiFi.softAPIP();
  server.begin();
}

void loop() {
  WiFiClient client = server.available();  // listen for incoming clients
  ServoSpeed(ServoPin1, speed1);
  ServoSpeed(ServoPin2, speed2);

  if (client) {
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print(controllerHTML);
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }

        if (currentLine.endsWith("GET /G")) {
          speed1 = 10;
          speed2 = -10;
        }
        if (currentLine.endsWith("GET /B")) {
          speed1 = -10;
          speed2 = 10;
        }
        if (currentLine.endsWith("GET /N")) {
          speed1 = 0;
          speed2 = 0;
        }
        if (currentLine.endsWith("GET /R")) {
          speed1 = 3;
          speed2 = 3;
        }
        if (currentLine.endsWith("GET /L")) {
          speed1 = -3;
          speed2 = -3;
        }
      }
    }
    client.stop();
  }
}