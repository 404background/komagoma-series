#include <WiFi.h>
#include <PubSubClient.h>
#include <NewPing.h>
#include <Wire.h>
#include <MPU6050.h>

const char* ssid = "<your SSID>";       // WiFiのSSIDに置き換えてください
const char* password = "<your password>";    // WiFiのパスワードに置き換えてください
const char* mqtt_server = "<your IP>>";  // MQTTブローカーのIPアドレスに置き換えてください

// サーボモータのピン設定
const int ServoPin1 = D1;
const int ServoPin2 = D2;
const int DutyMax = 2300;
const int DutyMin = 700;
int speed1 = 0;
int speed2 = 0;

// 超音波センサーのピン設定
const int triggerPin = D10;
const int echoPin = D9;
NewPing sonar(triggerPin, echoPin, 200);  // 最大距離は200cm

// MPU6050設定
MPU6050 mpu;
int16_t ax_offset = 0, ay_offset = 0, az_offset = 0;
int16_t gx_offset = 0, gy_offset = 0, gz_offset = 0;

WiFiClient espClient;
PubSubClient client(espClient);

// サーボモータの速度制御関数
void ServoSpeed(int pin, int speed) {  // speed: -10 ~ 10
  if (speed != 0) {
    int Duty = map(speed, -10, 10, DutyMin, DutyMax);
    digitalWrite(pin, HIGH);
    delayMicroseconds(Duty);
    digitalWrite(pin, LOW);
    delayMicroseconds(20000 - Duty);  // 20ms周期
  }
}

// MQTT再接続関数
void reconnect() {
  while (!client.connected()) {
    Serial.print("MQTT接続を試みています...");
    if (client.connect("ESP32Client")) {
      Serial.println("接続成功");
      client.subscribe("servo/control");  // サーボ制御のトピックを購読
    } else {
      Serial.print("失敗、rc=");
      Serial.print(client.state());
      Serial.println(" 5秒後に再試行します");
      delay(5000);
    }
  }
}

// MQTTのコールバック関数
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  if (message == "G") {
    speed1 = 10;
    speed2 = -10;
    Serial.println("\"G\"");
  } else if (message == "B") {
    speed1 = -10;
    speed2 = 10;
  } else if (message == "S") {
    speed1 = 0;
    speed2 = 0;
  } else if (message == "R") {
    speed1 = 3;
    speed2 = 3;
  } else if (message == "L") {
    speed1 = -3;
    speed2 = -3;
  }
}

void setup() {
  Serial.begin(115200);

  // サーボモータの初期化
  pinMode(ServoPin1, OUTPUT);
  pinMode(ServoPin2, OUTPUT);
  ServoSpeed(ServoPin1, 0);
  ServoSpeed(ServoPin2, 0);

  // MPU6050の初期化
  Wire.begin();
  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("MPU6050接続に失敗しました");
  }

  // MPU6050のキャリブレーション
  Serial.println("MPU6050をキャリブレーション中...");
  mpu.CalibrateAccel(6);
  mpu.CalibrateGyro(6);
  Serial.println("キャリブレーション完了");

  // WiFi接続
  Serial.print("WiFiに接続中");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFiに接続しました");

  // MQTTサーバー設定と接続
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  reconnect();
}

void loop() {
  // センサーデータをMQTTで送信
  char msg[50];

  ServoSpeed(ServoPin1, speed1);
  ServoSpeed(ServoPin2, speed2);

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // 超音波センサーから距離を取得
  float distance = sonar.ping_cm();
  if (distance == 0) distance = 200;  // 最大距離

  // 距離データ送信
  snprintf(msg, sizeof(msg), "%.2f", distance);
  client.publish("sensor/distance", msg);

  // MPU6050から加速度と角速度を取得
  int16_t ax_raw, ay_raw, az_raw;
  int16_t gx_raw, gy_raw, gz_raw;
  mpu.getAcceleration(&ax_raw, &ay_raw, &az_raw);
  mpu.getRotation(&gx_raw, &gy_raw, &gz_raw);

  // 加速度データをgに変換 (±2gレンジの場合)
  float ax = ax_raw / 16384.0;
  float ay = ay_raw / 16384.0;
  float az = az_raw / 16384.0;

  // 角速度データを°/sに変換 (±250°/sレンジの場合)
  float gx = gx_raw / 131.0;
  float gy = gy_raw / 131.0;
  float gz = gz_raw / 131.0;

  // 加速度データ送信
  snprintf(msg, sizeof(msg), "%.2f,%.2f,%.2f", ax, ay, az);
  client.publish("sensor/acceleration", msg);

  // 角速度データ送信
  snprintf(msg, sizeof(msg), "%.2f,%.2f,%.2f", gx, gy, gz);
  client.publish("sensor/gyro", msg);
}
