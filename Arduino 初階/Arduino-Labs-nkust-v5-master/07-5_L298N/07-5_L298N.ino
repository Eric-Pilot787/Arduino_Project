// =======================
// Motor + Potentiometer
// L298N Driver
// =======================

const int motorPinENA = 3;   // PWM
const int motorPinIN1 = 4;
const int motorPinIN2 = 5;

const int potPin = A0;

// ---------- Parameters ----------
const int DEAD_ZONE = 20;    // 最左20內停止
const int START_PWM = 120;   // 最低啟動速度(可依馬達調整)
const int MAX_PWM = 255;

const unsigned long RUN_TIME = 5000;   // 正反轉時間
const unsigned long STOP_TIME = 1000;  // 停止時間

// ---------- Variables ----------
bool clockwise = true;
bool motorRunning = true;

unsigned long previousMillis = 0;

void setup() {

  pinMode(motorPinENA, OUTPUT);
  pinMode(motorPinIN1, OUTPUT);
  pinMode(motorPinIN2, OUTPUT);

  Serial.begin(115200);

  previousMillis = millis();
}

void loop() {

  unsigned long currentMillis = millis();

  //==============================
  // 讀取可變電阻 (即時)
  //==============================

  int potValue = analogRead(potPin);

  int motorSpeed;

  if (potValue <= DEAD_ZONE) {
    motorSpeed = 0;
  }
  else {
    motorSpeed = map(potValue,
                     DEAD_ZONE,
                     1023,
                     START_PWM,
                     MAX_PWM);
  }

  //==============================
  // 狀態控制 (millis)
  //==============================

  if (motorRunning) {

    if (currentMillis - previousMillis >= RUN_TIME) {

      motorRunning = false;
      previousMillis = currentMillis;

      analogWrite(motorPinENA, 0);
    }

  }
  else {

    if (currentMillis - previousMillis >= STOP_TIME) {

      motorRunning = true;
      clockwise = !clockwise;

      previousMillis = currentMillis;
    }

  }

  //==============================
  // 控制方向
  //==============================

  if (clockwise) {
    digitalWrite(motorPinIN1, HIGH);
    digitalWrite(motorPinIN2, LOW);
  }
  else {
    digitalWrite(motorPinIN1, LOW);
    digitalWrite(motorPinIN2, HIGH);
  }

  //==============================
  // 控制速度 (即時)
  //==============================

  if (motorRunning) {

    analogWrite(motorPinENA, motorSpeed);

  } else {

    analogWrite(motorPinENA, 0);

  }

  //==============================
  // 除錯資訊
  //==============================

  static unsigned long serialTimer = 0;

  if (currentMillis - serialTimer >= 200) {

    serialTimer = currentMillis;

    Serial.print("Pot = ");
    Serial.print(potValue);

    Serial.print("  PWM = ");
    Serial.print(motorSpeed);

    Serial.print("  Direction = ");

    if (clockwise)
      Serial.println("CW");
    else
      Serial.println("CCW");
  }

}