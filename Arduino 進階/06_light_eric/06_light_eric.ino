//光敏電阻測試
int light = 36;
int GreenLight = 17;

void setup() {
  // put your setup code here, to run once:
  pinMode(light, INPUT);
  pinMode(GreenLight, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  int v = analogRead(light);
  Serial.println(v);
  if(v>2000){
    digitalWrite(GreenLight, HIGH);
  }else{
    digitalWrite(GreenLight, LOW);
  }
  delay(500);
}
