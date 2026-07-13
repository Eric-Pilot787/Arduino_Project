#include <string.h>

bool blinking = false;          // 是否為閃爍模式
unsigned long lastBlink = 0;    // 上次切換時間
bool ledState = LOW;            // LED目前狀態

void setup() {
  pinMode(3, OUTPUT);

  Serial.begin(9600);
  while (!Serial);

  Serial.println("Program Start!!");
}

void loop() {

  // ===== LED 閃爍功能 =====
  if (blinking) {
    if (millis() - lastBlink >= 100) {   // 每 500 ms 切換一次
      lastBlink = millis();
      ledState = !ledState;
      digitalWrite(3, ledState);
    }
  }

  static char buf[200];
  static int length = 0;
  int incoming_byte;

  if (Serial.available() > 0) {
    incoming_byte = Serial.read();
    buf[length++] = incoming_byte;

    if (incoming_byte == '\n') {
      buf[length] = '\0';

      if (strcmp(buf, "on\n") == 0) {
        blinking = false;                 // 關閉閃爍模式
        digitalWrite(3, HIGH);
        Serial.println("LED is on.");
      }
      else if (strcmp(buf, "off\n") == 0) {
        blinking = false;                 // 關閉閃爍模式
        digitalWrite(3, LOW);
        Serial.println("LED is off.");
      }
      else if (strcmp(buf, "shining\n") == 0) {
        blinking = true;                  // 開啟閃爍模式
        lastBlink = millis();
        Serial.println("LED is shining.");
      }
      else {
        Serial.print("Unknown command: ");
        Serial.println(buf);
      }

      length = 0;
    }
  }
}