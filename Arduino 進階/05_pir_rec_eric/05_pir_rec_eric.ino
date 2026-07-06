//人體感測器
int pir = 17;
int rec = 16;

void setup() {
  // put your setup code here, to run once:
  pinMode(pir, INPUT);
  pinMode(rec, OUTPUT);
  Serial.begin(115200);
  digitalWrite(rec, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  int value = digitalRead(pir);

  if (value == HIGH){
    Serial.println("有人經過");
    digitalWrite(rec,HIGH);
    delay(1000);
    digitalWrite(rec,LOW);
  }else{
    Serial.println("無人");
  }
  delay(500);


}
