// 利用RGB全彩燈，紅4 綠2 藍15
// 使用analogwrite 與 map 做數值轉換
// 紅燈 3 秒、黃燈 1 秒、綠燈 3 秒
// 順序 綠->黃->紅

const int GREEN_PIN = 2;
const int BLUE_PIN = 15;
const int RED_PIN = 4;
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
