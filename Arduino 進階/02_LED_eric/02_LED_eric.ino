const int RED_PIN = 4;
const int GREEN_PIN = 2;
const int BLUE_PIN = 15;
const int LEVEL_MS = 1000;

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
}

void setColor(int redPercent, int greenPercent, int bluePercent) {
  analogWrite(RED_PIN, map(redPercent, 0, 100, 0, 255));
  analogWrite(GREEN_PIN, map(greenPercent, 0, 100, 0, 255));
  analogWrite(BLUE_PIN, map(bluePercent, 0, 100, 0, 255));
}

void loop() {
  setColor(0, 100, 0);   // 綠燈 3 秒
  delay(3000);

  setColor(100, 50, 0);  // 黃燈 1 秒
  delay(1000);

  setColor(100, 0, 0);   // 紅燈 3 秒
  delay(3000);
}