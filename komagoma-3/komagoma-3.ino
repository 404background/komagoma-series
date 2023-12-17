#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include "SR04.h"

const int ServoPin1 = D1;
const int ServoPin2 = D2;
const int TRIG_PIN = D10;
const int ECHO_PIN = D9;

int speed1 = 0;
int speed2 = 0;
#define DutyMax 2300
#define DutyMin 700

SR04 sr04 = SR04(ECHO_PIN, TRIG_PIN);
long a;
int initMillis = 0;

const char *ssid = "ESP32C3";
const char *password = "123456789";
WiFiServer server(80);

String str =
  "Click <a href=\"/H\">here</a> to Gp straight.<br>"
  "Click <a href=\"/L\">here</a> to Go back.<br>"
  "Click <a href=\"/N\">here</a> to Stop.<br>"
  "Click <a href=\"/R\">here</a> to turn right.<br>"
  "Click <a href=\"/L\">here</a> to turn left.<br>";

String controller =
  "<!DOCTYPE html>\
<html lang=\"ja\">\
<head>\
    <meta charset=\"UTF-8\">\
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, minimum-scale=1.0, user-scalable=no\">\
    <style>\
        .dan{\
            display: flex;\
            justify-content: center;\
        }\
        .button {\
            width: 30vw;\
            aspect-ratio: 1/1;\
            background-color: #3498db;\
            text-align: center;\
            color: white;\
            font-size: 10vw;\
            border-radius: 5%;\
            margin: 5vw;\
            display: inline-block;\
            cursor: pointer;\
            user-select: none;\
        }\
        .active{\
            background-color: #2980b9;\
            transform: translateY(2%);\
        }\
    </style>\
</head>\
<body>\
    0<input type=\"range\" name=\"speed\" min=\"0\" max=\"10\">10\
    <div class=\"dan\">\
        <div class=\"button\" id=\"up\"><p>前進⇧</p></div>\
    </div>\
    <div class=\"dan\">\
        <div class=\"button\" id=\"left\"><p>⇦左回転</p></div>\
        <div class=\"button\" id=\"stop\"><p>停止</p></div>\
        <div class=\"button\" id=\"right\"><p>右回転⇨</p></div>\
    </div>\
    <div class=\"dan\">\
        <div class=\"button\" id=\"down\"><p>後進⇩</p></div>\
    </div>\
</body>\
</html>";

void ServoSpeed(int pin, int speed) {
  int Duty = map(speed, -10, 10, DutyMin, DutyMax);
  digitalWrite(pin, HIGH);
  delayMicroseconds(Duty);
  digitalWrite(pin, LOW);
  delayMicroseconds(20 * 1000 - Duty);
}

void setup() {
  Serial.begin(115200);
  pinMode(ServoPin1, OUTPUT);
  pinMode(ServoPin2, OUTPUT);
  ServoSpeed(ServoPin1, 5);
  ServoSpeed(ServoPin2, 5);
  initMillis = millis();

  while (!WiFi.softAP(ssid, password)) {}
  IPAddress myIP = WiFi.softAPIP();
  server.begin();
}

void loop() {
  /*
  a = sr04.Distance();
  Serial.print(a);
  Serial.println("cm");
  if (millis() - initMillis > 100) {
    speed = -10;
    for (int i = 0; i < 1000; i++) {
      ServoSpeed(ServoPin1, speed);
      ServoSpeed(ServoPin2, speed * -1);
    }
  }
  */
  WiFiClient client = server.available();  // listen for incoming clients
  ServoSpeed(ServoPin1, speed1);
  ServoSpeed(ServoPin2, speed2);

  if (client) {                   // if you get a client,
    String currentLine = "";      // make a String to hold incoming data from the client
    while (client.connected()) {  // loop while the client's connected
      if (client.available()) {   // if there's bytes to read from the client,
        char c = client.read();   // read a byte, then
        if (c == '\n') {          // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print(str);

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {  // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        if (currentLine.endsWith("GET /H")) {
          speed1 = 5;
          speed2 = 5;
        }
        if (currentLine.endsWith("GET /L")) {
          speed1 = -5;
          speed2 = -5;
        }
        if (currentLine.endsWith("GET /N")) {
          speed1 = 0;
          speed2 = 0;
        }
        if (currentLine.endsWith("GET /R")) {
          speed1 = -5;
          speed2 = 5;
        }
        if (currentLine.endsWith("GET /L")) {
          speed1 = 5;
          speed2 = -5;
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}