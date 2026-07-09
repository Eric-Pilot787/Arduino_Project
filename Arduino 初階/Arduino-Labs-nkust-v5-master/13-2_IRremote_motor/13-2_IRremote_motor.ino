#include <Arduino.h>
#include <IRremote.h>

#define IR_RECEIVE_PIN 2

#define ENA 3
#define IN1 4
#define IN2 5

#define SPEED1 80
#define SPEED2 160
#define SPEED3 255

void motorForward(int speed)
{
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, speed);
}

void motorBackward(int speed)
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, speed);
}

void motorStop()
{
  analogWrite(ENA, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
}

void setup()
{
  Serial.begin(115200);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  motorStop();

  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  Serial.println("IR Motor Control Ready");
}

void loop()
{
  if (IrReceiver.decode())
  {
    byte command = IrReceiver.decodedIRData.command;

    Serial.print("Command = 0x");
    Serial.println(command, HEX);

    switch (command)
    {
      case 0x16:      // 0
        motorStop();
        break;

      case 0x0C:      // 1
        motorForward(SPEED1);
        break;

      case 0x18:      // 2
        motorForward(SPEED2);
        break;

      case 0x5E:      // 3
        motorForward(SPEED3);
        break;

      case 0x08:      // 4
        motorBackward(SPEED1);
        break;

      case 0x1C:      // 5
        motorBackward(SPEED2);
        break;

      case 0x5A:      // 6
        motorBackward(SPEED3);
        break;

      default:
        break;
    }

    IrReceiver.resume();
  }
}