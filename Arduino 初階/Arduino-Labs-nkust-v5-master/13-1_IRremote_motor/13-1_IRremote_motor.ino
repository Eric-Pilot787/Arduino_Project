#include <Arduino.h>
#include <IRremote.h>

#define IR_RECEIVE_PIN  2
#define SPEED1  80
#define SPEED2  160
#define SPEED3  240

const int motorPinENA = 6;
const int motorPinIN1 = 4;
const int motorPinIN2 = 5;

void setup() {
  pinMode(motorPinENA, OUTPUT);
  pinMode(motorPinIN1, OUTPUT);
  pinMode(motorPinIN2, OUTPUT);
  
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Start!!");
  
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK, USE_DEFAULT_FEEDBACK_LED_PIN);
  Serial.print(F("Ready to receive IR signals at pin "));
  Serial.println(IR_RECEIVE_PIN);

  analogWrite(motorPinENA, 0);
  digitalWrite(motorPinIN1, HIGH);
  digitalWrite(motorPinIN2, LOW);
}

void loop() {
  byte command;
  if (IrReceiver.decode()) {
    // Print a short summary of received data
    IrReceiver.printIRResultShort(&Serial);
    if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
      // We have an unknown protocol here, print more info
      IrReceiver.printIRResultRawFormatted(&Serial, true);
    }
    Serial.println();
//    IrReceiver.resume(); // Enable receiving of the next value
    command = IrReceiver.decodedIRData.command;
    Serial.print("Command is ");
    Serial.println(command, HEX);
//#if 0
    switch(command) {
      case 0x16:  // 0
        analogWrite(motorPinENA, 0);
      break;
      case 0xC:   // 1
        analogWrite(motorPinENA, SPEED1);
      break;
      case 0x18:  // 2
        analogWrite(motorPinENA, SPEED2);
      break;      
      case 0x5E:  // 3
        analogWrite(motorPinENA, SPEED3);
      break;
      default: {} 
    }
//#endif
    IrReceiver.resume(); // Enable receiving of the next value
  }
}
