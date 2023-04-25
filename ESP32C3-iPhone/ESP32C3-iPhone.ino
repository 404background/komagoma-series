#include "Freenove_WS2812_Lib_for_ESP32.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define LEDS_COUNT  8
#define LEDS_PIN  8
#define CHANNEL   0

Freenove_ESP32_WS2812 strip = Freenove_ESP32_WS2812(LEDS_COUNT, LEDS_PIN, CHANNEL, TYPE_GRB);

volatile char cmd;
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
  strip.begin();
  strip.setBrightness(20);

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
}

void loop() {
  Serial.println("waiting");
  delay(1000);
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
    /*Check battery voltage****************************************************************************/
    case '0':
      while (!Serial.available()) {
        for (int i = 0; i < 16; i++) {
          Vbatt = Vbatt + analogReadMilliVolts(A0); // ADC with correction
        }
        Vbattf = 2 * Vbatt / 16 / 1000.0;     // attenuation ratio 1/2, mV --> V
        Serial.println(Vbattf, 3);
        break;
        //default:
        //strip.setAllLedsColor(0);
      }
  }
}
