#include <PS4Controller.h>

void setup() {
  Serial.begin(115200);
  PS4.begin();
  Serial.println("Ready.");
}

void loop() {
  // Below has all accessible outputs from the controller
  if (PS4.isConnected()) {
    /*
      if (PS4.Right()) Serial.println("Right Button");
      if (PS4.Down()) Serial.println("Down Button");
      if (PS4.Up()) Serial.println("Up Button");
      if (PS4.Left()) Serial.println("Left Button");

      if (PS4.Square()) Serial.println("Square Button");
      if (PS4.Cross()) Serial.println("Cross Button");
      if (PS4.Circle()) Serial.println("Circle Button");
      if (PS4.Triangle()) Serial.println("Triangle Button");

      if (PS4.UpRight()) Serial.println("Up Right");
      if (PS4.DownRight()) Serial.println("Down Right");
      if (PS4.UpLeft()) Serial.println("Up Left");
      if (PS4.DownLeft()) Serial.println("Down Left");

      if (PS4.L1()) Serial.println("L1 Button");
      if (PS4.R1()) Serial.println("R1 Button");

      if (PS4.Share()) Serial.println("Share Button");
      if (PS4.Options()) Serial.println("Options Button");
      if (PS4.L3()) Serial.println("L3 Button");
      if (PS4.R3()) Serial.println("R3 Button");

      if (PS4.PSButton()) Serial.println("PS Button");
      if (PS4.Touchpad()) Serial.println("Touch Pad Button");

      if (PS4.L2()) {
      Serial.printf("L2 button at %d\n", PS4.L2Value());
      }
      if (PS4.R2()) {
      Serial.printf("R2 button at %d\n", PS4.R2Value());
      }

      if (PS4.LStickX()) {
      Serial.printf("Left Stick x at %d\n", PS4.LStickX());
      }
      if (PS4.LStickY()) {
      Serial.printf("Left Stick y at %d\n", PS4.LStickY());
      }
      if (PS4.RStickX()) {
      Serial.printf("Right Stick x at %d\n", PS4.RStickX());
      }
    */
    if (PS4.RStickY()) {
      Serial.printf("Right Stick y at %d\n", PS4.RStickY());
      if (PS4.RStickY()) {
        Serial.printf("Right Stick y at %d\n", PS4.RStickY());
        //MOTOR1.setSpeed(PS4.LStickY());

        //スティックの傾きに合わせて、回転速度を調整
        int RStickY = map(PS4.RStickY(), -128, 128, -256, 256);
        if (RStickY <= 20 && RStickY >= -20) {
          ledcWrite(0, 0);
        } else {
          ledcWrite(0, abs(RStickY));
        }
        //回転方向
        if (RStickY >= 0) {
          digitalWrite(DIR1, HIGH);
        } else {
          digitalWrite(DIR1, LOW);
        }
      }
    }
    /*
      if (PS4.Charging()) Serial.println("The controller is charging");
      if (PS4.Audio()) Serial.println("The controller has headphones attached");
      if (PS4.Mic()) Serial.println("The controller has a mic attached");

      Serial.printf("Battery Level : %d\n", PS4.Battery());
    */
  }
}
