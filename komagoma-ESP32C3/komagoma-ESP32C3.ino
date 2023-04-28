//A0:バッテリー電圧確認用（https://wiki.seeedstudio.com/XIAO_ESP32C3_Getting_Started/）

#include "BNO055.h"
#include "Freenove_WS2812_Lib_for_ESP32.h"
#include "SoftwareSerial.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define LEDS_COUNT  8
#define LEDS_PIN  8
#define CHANNEL   0

Freenove_ESP32_WS2812 strip = Freenove_ESP32_WS2812(LEDS_COUNT, LEDS_PIN, CHANNEL, TYPE_GRB);
//esp_err_t setLedColor(int index, uint8_t r, uint8_t g, uint8_t b);

String command = "";

int AIN1 = D3;     // 1つ目のDCモーターの制御
int AIN2 = D2;     // 1つ目のDCモーターの制御
int BIN1 = D1;     // 2つ目のDCモーターの制御
int BIN2 = D6;     // 2つ目のDCモーターの制御
int PWMA = D9;     // 1つ目のDCモーターの回転速度
int PWMB = D10;     // 2つ目のDCモーターの回転速度
int LED = D8;

volatile char cmd;     //for switch-case
uint32_t Vbatt = 0;
float Vbattf = 0;

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {
        Serial.println("*********");
        Serial.print("New value: ");
        for (int i = 0; i < value.length(); i++) {
          Serial.print(value[i]);
          cmd = value[i];

          Serial.println();
          Serial.println("*********");
        }
      }
    }
};

SoftwareSerial mySerial;

void subProcess(void * pvParameters) {
  Serial.begin(115200);
  while (1) {
    while (1) {
      if (Serial.available()) {
        cmd = Serial.read();
      }
    }
  }
};

void setup() {
  Serial.begin(115200);
  mySerial.begin(115200);
  pinMode(A0, INPUT);         // ADC
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);
  digitalWrite(PWMA, HIGH);
  digitalWrite(PWMB, HIGH);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  //Speed(100);               //モーターの速度設定（0～255）

  strip.begin();
  strip.setBrightness(20);

  BNO055_setup();
  BLEDevice::init("MyESP32");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->setValue("Hello World");
  pService->start();
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
  xTaskCreatePinnedToCore(subProcess, "subProcess", 4096, NULL, 1, NULL, 0); //Core 0でタスク開始
  strip.setLedColor(0, 255, 0, 0);
  delay(1000);
  strip.setLedColor(0, 0, 255, 0);
}

void loop() {
  if (Serial.available()) {
    cmd = Serial.read();
  }
  if (mySerial.available()) {
    cmd = mySerial.read();
  }

  switch (cmd) {
    case '1':
      while (cmd == '1') {
        for (int j = 0; j < 255; j += 2) {
          for (int i = 0; i < LEDS_COUNT; i++) {
            strip.setLedColorData(i, strip.Wheel((i * 256 / LEDS_COUNT + j) & 255));
          }
          strip.show();
          delay(10);
        }
      }
      break;
    case 'w':
      Move('w');
      break;
    case 's':
      Move(' ');
      strip.setLedColor(0, 255, 0, 0);
      break;
    case 'b':
      Move('s');
      break;
    case 'r':
      Move('d');
      break;
    case 'l':
      Move('a');
      break;
    /*Check battery voltage****************************************************************************/
    case '0':
      for (int i = 0; i < 16; i++) {
        Vbatt = Vbatt + analogReadMilliVolts(A0); // ADC with correction
      }
      Vbattf = 2 * Vbatt / 16 / 1000.0;     // attenuation ratio 1/2, mV --> V
      Serial.println(Vbattf, 3);
      cmd = ' ';
      break;
      //default:
      //strip.setAllLedsColor(0);
  }
}

/*
  void Speed(int x) {
  analogWrite(PWMA, x);
  analogWrite(PWMB, x);
  }
*/

void Move(int i) {
  switch (i) {
    case ' ': //停止
      digitalWrite(AIN1, LOW);
      digitalWrite(AIN2, LOW);
      digitalWrite(BIN1, LOW);
      digitalWrite(BIN2, LOW);
      break;
    case 'd': // 右回転
      digitalWrite(AIN1, HIGH);
      digitalWrite(AIN2, LOW);
      digitalWrite(BIN1, HIGH);
      digitalWrite(BIN2, LOW);
      break;
    case 'a': // 左回転
      digitalWrite(AIN1, LOW);
      digitalWrite(AIN2, HIGH);
      digitalWrite(BIN1, LOW);
      digitalWrite(BIN2, HIGH);
      break;
    case 'w': // 前進
      digitalWrite(AIN1, LOW);
      digitalWrite(AIN2, HIGH);
      digitalWrite(BIN1, HIGH);
      digitalWrite(BIN2, LOW);
      break;
    case 's': // 後進
      digitalWrite(AIN1, HIGH);
      digitalWrite(AIN2, LOW);
      digitalWrite(BIN1, LOW);
      digitalWrite(BIN2, HIGH);
      break;
  }
}
