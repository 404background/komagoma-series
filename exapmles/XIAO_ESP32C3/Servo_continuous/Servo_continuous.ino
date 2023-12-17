const int servo1 = 26;
const int servo2 = 17;
int DutyMax = 2300;  //180:2500  conitnuous:2300
int DutyMin = 700;   //180:500   continuout:700
int DutyCycle = DutyMin;
int Interval = 2000;

void setup() {
  pinMode(servo1, OUTPUT);
  pinMode(servo2, OUTPUT);
}

void loop() {
  for (DutyCycle = DutyMin; DutyCycle <= DutyMax; DutyCycle += 1) {
    Serial.println(DutyCycle);
    digitalWrite(servo1, HIGH);
    digitalWrite(servo2, HIGH);
    delayMicroseconds(DutyCycle);
    digitalWrite(servo1, LOW);
    digitalWrite(servo2, LOW);
    delayMicroseconds(20 * 1000 - DutyCycle);
  }
  delay(Interval);
  for (DutyCycle = DutyMax; DutyCycle >= DutyMin; DutyCycle -= 1) {
    Serial.println(DutyCycle);
    digitalWrite(servo1, HIGH);
    digitalWrite(servo2, HIGH);
    delayMicroseconds(DutyCycle);
    digitalWrite(servo1, LOW);
    digitalWrite(servo2, LOW);
    delayMicroseconds(20 * 1000 - DutyCycle);
  }
  delay(Interval);
}