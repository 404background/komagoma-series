#define TIMER_INTERRUPT_DEBUG       1
#define ISR_SERVO_DEBUG             1

#include "NRF52_ISR_Servo.h"

// Published values for SG90 servos; adjust if needed
#define MIN_MICROS        800
#define MAX_MICROS        2450

#define SERVO_PIN_1       A0
#define SERVO_PIN_2       A1
#define VOLUME_PIN_1       A2
#define SERVO_PIN_4       A3
#define SERVO_PIN_5       A4
#define SERVO_PIN_6       A5

typedef struct {
  int     servoIndex;
  uint8_t servoPin;
} ISR_servo_t;

#define NUM_SERVOS            2

ISR_servo_t ISR_servo[] = {
  { -1, SERVO_PIN_1 }, { -1, SERVO_PIN_2 }
};

void setup() {
  Serial.begin(115200);
  pinMode(VOLUME_PIN_1, INPUT);

  while (!Serial && millis() < 5000);

  delay(200);

  Serial.print(F("\nStarting NRF52_MultipleServos on ")); Serial.println(BOARD_NAME);
  Serial.println(NRF52_ISR_SERVO_VERSION);

  for (int index = 0; index < NUM_SERVOS; index++) {
    ISR_servo[index].servoIndex = NRF52_ISR_Servos.setupServo(ISR_servo[index].servoPin, MIN_MICROS, MAX_MICROS);

    if (ISR_servo[index].servoIndex != -1) {
      Serial.print(F("Setup OK, Servo index = "));
      Serial.println(ISR_servo[index].servoIndex);
      NRF52_ISR_Servos.setPosition(ISR_servo[index].servoIndex, 0);
    } else {
      Serial.print(F("Setup Failed, Servo index = "));
      Serial.println(ISR_servo[index].servoIndex);
    }
  }
}

void loop() {
  int position;      // position in degrees
  position = map(analogRead(VOLUME_PIN_1), 0, 1023, 0, 180);
  //for (position = 0; position <= 180; position += 10)
  //{
  // goes from 0 degrees to 180 degrees
  // in steps of 10 degree
  for (int index = 0; index < NUM_SERVOS; index++)
  {
    NRF52_ISR_Servos.setPosition(ISR_servo[index].servoIndex, position);
  }

  // waits 1s for the servo to reach the position
  delay(1000);
  //}

  delay(5000);
}
