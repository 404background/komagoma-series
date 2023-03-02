//コマゴマ一号　パソコンから無線で操作時
#include <SoftwareSerial.h>
#include<Wire.h>
#include "SR04.h"

//XBee用
SoftwareSerial mySerial(A0, A1);
char mes = "";

//HC-SR04用
#define TRIG_PIN 12  
#define ECHO_PIN 11
SR04 sr04 = SR04(ECHO_PIN,TRIG_PIN);
long sonic;

//MPU6050用
const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

//TB6612FNG用
const int AIN1 = 5;     // 1つ目のDCモーターの制御
const int AIN2 = 4;     // 1つ目のDCモーターの制御
const int BIN1 = 7;     // 2つ目のDCモーターの制御
const int BIN2 = 8;     // 2つ目のDCモーターの制御
const int PWMA = 3;     // 1つ目のDCモーターの回転速度
const int PWMB = 9;    // 2つ目のDCモーターの回転速度


void setup() {
  Serial.begin(9600);
  Serial.println("Serial connected");
  mySerial.begin(9600);
  mySerial.println("mySerial connected");
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
}

void loop() {
  //モーター制御
  char cmd;
  if(mySerial.available()){
    cmd = mySerial.read();
    mySerial.print(cmd);
    Move(cmd);
  }

  //超音波センサー
  int distance = Sonic();
  if(distance < 15) {
    Move('5');
    mySerial.println("DANGER!!!");
    Move('2');
    Move('5');
  }

  //加速度センサー 
  MPU6050();
}

int Sonic() {
  sonic=sr04.Distance();
  Serial.print(sonic);
  Serial.println("cm ");
  mySerial.print(sonic);
  mySerial.print("cm ");
  return sonic;
}

void Move(int i) {
  switch(i) {
    case '5': //停止
      digitalWrite(AIN1, LOW);
      digitalWrite(AIN2, LOW);
      digitalWrite(BIN1, LOW);
      digitalWrite(BIN2, LOW);
      mySerial.println("stop");
      break;
    case '6': // 右回転
      digitalWrite(AIN1, HIGH);
      digitalWrite(AIN2, LOW);
      digitalWrite(BIN1, LOW);
      digitalWrite(BIN2, HIGH);
      mySerial.println("turn right");
      rpm(80);
      break;
    case '4': // 左回転
      digitalWrite(AIN1, LOW);
      digitalWrite(AIN2, HIGH);
      digitalWrite(BIN1, HIGH);
      digitalWrite(BIN2, LOW);
      mySerial.println("turn left");
      rpm(80);
      break;
    case '8': // 前進
      digitalWrite(AIN1, HIGH);
      digitalWrite(AIN2, LOW);
      digitalWrite(BIN1, HIGH);
      digitalWrite(BIN2, LOW);
      mySerial.println("go straight");
      rpm(200);
      break;
    case '2': // 後進
      
      digitalWrite(AIN1, LOW);
      digitalWrite(AIN2, HIGH);
      digitalWrite(BIN1, LOW);
      digitalWrite(BIN2, HIGH);
      mySerial.println("go back");
      rpm(100);
      break;
  }
}

//rpm for TB6612FNG
int rpm(int x) {
  analogWrite(PWMA, x);
  analogWrite(PWMB, x);
}

void MPU6050() {
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
/*
  Serial.print("AcX = "); Serial.print(AcX);
  Serial.print(" | AcY = "); Serial.print(AcY);
  Serial.print(" | AcZ = "); Serial.print(AcZ);
  Serial.print(" | Tmp = "); Serial.print(Tmp/340.00+36.53);  //equation for temperature in degrees C from datasheet
  Serial.print(" | GyX = "); Serial.print(GyX);
  Serial.print(" | GyY = "); Serial.print(GyY);
  Serial.print(" | GyZ = "); Serial.println(GyZ);
*/
  //from XBee, SoftwareSerial
  mySerial.print("AcX = "); mySerial.print(AcX);
  mySerial.print(" | AcY = "); mySerial.print(AcY);
  mySerial.print(" | AcZ = "); mySerial.print(AcZ);
  mySerial.print(" | Tmp = "); mySerial.print(Tmp/340.00+36.53);  //equation for temperature in degrees C from datasheet
  mySerial.print(" | GyX = "); mySerial.print(GyX);
  mySerial.print(" | GyY = "); mySerial.print(GyY);
  mySerial.print(" | GyZ = "); mySerial.println(GyZ);
}
