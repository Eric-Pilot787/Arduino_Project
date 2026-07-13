const int Red = 9;
const int Green = 10;
const int Blue = 11;
const int Pot = A0;

void setup() {
  pinMode(Red, OUTPUT);
  pinMode(Green, OUTPUT);
  pinMode(Blue, OUTPUT);
}

void loop() {

  int value = analogRead(Pot);

  // 0~1023 對應到 0~1530 (6段，每段255)
  int color = map(value, 0, 1023, 0, 1530);

  int r = 0, g = 0, b = 0;

  if (color < 255) {
    r = 255;
    g = color;
    b = 0;
  }
  else if (color < 510) {
    r = 510 - color;
    g = 255;
    b = 0;
  }
  else if (color < 765) {
    r = 0;
    g = 255;
    b = color - 510;
  }
  else if (color < 1020) {
    r = 0;
    g = 1020 - color;
    b = 255;
  }
  else if (color < 1275) {
    r = color - 1020;
    g = 0;
    b = 255;
  }
  else {
    r = 255;
    g = 0;
    b = 1530 - color;
  }

  // 共陽極要反轉
  analogWrite(Red, 255 - r);
  analogWrite(Green, 255 - g);
  analogWrite(Blue, 255 - b);

  delay(10);
}