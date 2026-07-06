//檢測空氣中可燃氣體  3120下   3200上
int GAS = 36;

int RedLight = 15;
int GreenLight = 2;
int BlueLight = 4;

void setup() {
  // put your setup code here, to run once:
  pinMode(GAS, INPUT);
  pinMode(RedLight, OUTPUT);
  pinMode(GreenLight, OUTPUT);
  pinMode(BlueLight, OUTPUT);
  Serial.begin(115200);
}

void setColor(int redPercent, int greenPercent, int bluePercent) {
  analogWrite(RedLight, map(redPercent, 0, 100, 0, 255));
  analogWrite(GreenLight, map(greenPercent, 0, 100, 0, 255));
  analogWrite(BlueLight, map(bluePercent, 0, 100, 0, 255));
}

void loop() {
  // put your main code here, to run repeatedly:
  int GASvalue = analogRead(GAS);
  Serial.println(GASvalue);
  if(GASvalue>3100){
    setColor(100, 0, 0);   // 紅燈
    Serial.println("通通抓去關");
  }else if(GASvalue >= 3000 && GASvalue <= 3100){
    setColor(100, 100, 0);   // 黃燈
    Serial.println("小酌");
  }else{
    setColor(0, 100, 0);
    Serial.println("很乖 沒喝9");
  }
  delay(500);
}
//why can't light up yellow