const int GREEN_PIN = 15;
const int YELLOW_PIN = 2;
const int RED_PIN = 4;
const int BEAT_MS = 500;  // 4/4 拍，120 BPM 時每拍 500 ms

void setup() {
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
}

void loop() {
  // 拍 1：綠燈
  digitalWrite(GREEN_PIN, HIGH);
  digitalWrite(YELLOW_PIN, LOW);
  digitalWrite(RED_PIN, LOW);
  delay(BEAT_MS);

  // 拍 2：黃燈
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(YELLOW_PIN, HIGH);
  digitalWrite(RED_PIN, LOW);
  delay(BEAT_MS);

  // 拍 3：紅燈
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(YELLOW_PIN, LOW);
  digitalWrite(RED_PIN, HIGH);
  delay(BEAT_MS);

  // 拍 4：全部熄
  // digitalWrite(GREEN_PIN, LOW);
  // digitalWrite(YELLOW_PIN, LOW);
  // digitalWrite(RED_PIN, LOW);
  // delay(BEAT_MS);
}