#include <Keypad.h>

const int buzzerPin = 10;

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

char melodyKeys[16] = {
  '1', '2', '3', 'A',
  '4', '5', '6', 'B',
  '7', '8', '9', 'C',
  '*', '0', '#', 'D'
};

// 你的旋律：
// A4 A4 G4 G4 A4 G4 C4 D4 E4 F4 G4 A4 G4 D4
int melodyNotes[16] = {
  440,  // 1 = A4
  440,  // 2 = A4
  392,  // 3 = G4
  392,  // A = G4

  440,  // 4 = A4
  392,  // 5 = G4
  262,  // 6 = C4
  294,  // B = D4

  330,  // 7 = E4
  349,  // 8 = F4
  392,  // 9 = G4
  440,  // C = A4

  392,  // * = G4
  294,  // 0 = D4
  0,    // # = 不發聲
  0     // D = 不發聲
};

void setup() {
  Serial.begin(9600);
  pinMode(buzzerPin, OUTPUT);

  Serial.println("4x4 Melody Keyboard Ready");
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    Serial.print("Pressed Key: ");
    Serial.println(key);

    for (int i = 0; i < 16; i++) {
      if (key == melodyKeys[i]) {

        if (melodyNotes[i] > 0) {
          tone(buzzerPin, melodyNotes[i], 300);

          Serial.print("Frequency: ");
          Serial.println(melodyNotes[i]);
        } else {
          noTone(buzzerPin);
          Serial.println("No sound");
        }

        break;
      }
    }
  }
}