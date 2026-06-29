// 讓綠燈(15腳)以三個狀態來改變亮度
// 0-不亮
// 50-中亮
// 100-全亮
// 每個亮度各一秒，重複不斷

const int GREEN_PIN = 15;
const int YELLOW_PIN = 2;
const int RED_PIN = 4;
const int LEVEL_MS = 1000;

void setup() {
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
}

void loop() {

  // int brightness = 0;
  // analogWrite(GREEN_PIN, map(brightness, 0, 100, 0, 255));
  // delay(LEVEL_MS);

  // brightness = 50;
  // analogWrite(GREEN_PIN, map(brightness, 0, 100, 0, 255));
  // delay(LEVEL_MS);

  // brightness = 100;
  // analogWrite(GREEN_PIN, map(brightness, 0, 100, 0, 255));
  // delay(LEVEL_MS);

  for (int brightness = 0; brightness <= 100; brightness++) {
    analogWrite(GREEN_PIN, map(brightness, 0, 100, 0, 255));
    delay(50);
  }

  for (int brightness = 100; brightness >= 0; brightness--) {
    analogWrite(GREEN_PIN, map(brightness, 0, 100, 0, 255));
    delay(50);
  }

}
