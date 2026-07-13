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

// D 鍵用來切換下一組，不播放音
char playKeys[15] = {
  '1', '2', '3', 'A',
  '4', '5', '6', 'B',
  '7', '8', '9', 'C',
  '*', '0', '#'
};

// =======================
// C3 到 B6 頻率表
// =======================
#define NOTE_C3    131
#define NOTE_CS3   139
#define NOTE_D3    147
#define NOTE_DS3   156
#define NOTE_E3    165
#define NOTE_F3    175
#define NOTE_FS3   185
#define NOTE_G3    196
#define NOTE_GS3   208
#define NOTE_A3    220
#define NOTE_AS3   233
#define NOTE_B3    247

#define NOTE_C4    262
#define NOTE_CS4   277
#define NOTE_D4    294
#define NOTE_DS4   311
#define NOTE_E4    330
#define NOTE_F4    349
#define NOTE_FS4   370
#define NOTE_G4    392
#define NOTE_GS4   415
#define NOTE_A4    440
#define NOTE_AS4   466
#define NOTE_B4    494

#define NOTE_C5    523
#define NOTE_CS5   554
#define NOTE_D5    587
#define NOTE_DS5   622
#define NOTE_E5    659
#define NOTE_F5    698
#define NOTE_FS5   740
#define NOTE_G5    784
#define NOTE_GS5   831
#define NOTE_A5    880
#define NOTE_AS5   932
#define NOTE_B5    988

#define NOTE_C6    1047
#define NOTE_CS6   1109
#define NOTE_D6    1175
#define NOTE_DS6   1245
#define NOTE_E6    1319
#define NOTE_F6    1397
#define NOTE_FS6   1480
#define NOTE_G6    1568
#define NOTE_GS6   1661
#define NOTE_A6    1760
#define NOTE_AS6   1865
#define NOTE_B6    1976

#define NO_NOTE 0

const byte GROUP_COUNT = 12;
const byte NOTE_PER_GROUP = 15;

// ==================================================
// 只保留「歌詞對應的音」
// 不放休止符、不放連音線延長音
//
// 每組最多 15 個音：
// 1 2 3 A 4 5 6 B 7 8 9 C * 0 #
// D = 切換下一組
// ==================================================

int melody[GROUP_COUNT][NOTE_PER_GROUP] = {

  // 第 1 組
  // 素眉勾勒 秋千畫 筆鋒濃轉淡
  // D4 C4 A3 C4 C4 A3 C4 C4 A3 C4 A3 G3
  {
    NOTE_D4, NOTE_C4, NOTE_A3, NOTE_C4,
    NOTE_C4, NOTE_A3, NOTE_C4, NOTE_C4,
    NOTE_A3, NOTE_C4, NOTE_A3, NOTE_G3,
    NO_NOTE, NO_NOTE, NO_NOTE
  },

  // 第 2 組
  {
    NOTE_C4, NOTE_C4, NOTE_A3, NOTE_C4,
    NOTE_C4, NOTE_E4, NOTE_D4, NOTE_C4,
    NOTE_C4, NOTE_G3, NOTE_A3, NOTE_E4,
    NO_NOTE, NO_NOTE, NO_NOTE
  },

  // 第 3 組
  {
    NOTE_E4, NOTE_E4, NOTE_D4, NOTE_E4,
    NOTE_E4, NOTE_D4, NOTE_E4, NOTE_G4,
    NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4,
    NO_NOTE, NO_NOTE, NO_NOTE
  },

  // 第 4 組
  {
    NOTE_D4, NOTE_D4, NOTE_D4, NOTE_D4,
    NOTE_D4, NOTE_C4, NOTE_E4, NOTE_E4,
    NOTE_D4, NOTE_D4, NOTE_C4, NOTE_A3,
    NO_NOTE, NO_NOTE, NO_NOTE
  },

  // 第 5 組
  {
    NOTE_C4, NOTE_C4, NOTE_A3, NOTE_C4,
    NOTE_C4, NOTE_A3, NOTE_G3, NOTE_G3,
    NOTE_A3, NOTE_E4,
    NO_NOTE, NO_NOTE, NO_NOTE, NO_NOTE, NO_NOTE
  },

  // 第 6 組
  {
    NOTE_G4, NOTE_G4, NOTE_E4, NOTE_G4,
    NOTE_G4, NOTE_E4, NOTE_D4, NOTE_C4,
    NOTE_C4, NOTE_D4, NOTE_C4, NOTE_D4,
    NO_NOTE, NO_NOTE, NO_NOTE
  },

  // 第 7 組
  {
    NOTE_E4, NOTE_D4, NOTE_D4, NOTE_C4,
    NOTE_D4, NOTE_C4, NOTE_A3, NOTE_D4,
    NOTE_C4, NOTE_C4, NOTE_A3, NOTE_C4,
    NOTE_C4, NOTE_C4, NO_NOTE
  },

  // 第 8 組
  // 天青色等煙雨 而我在等你
  // 修正版：天、青 = G4，色 = E4
  {
    NOTE_G4, NOTE_G4, NOTE_E4, NOTE_D4,
    NOTE_E4, NOTE_A3, NOTE_D4, NOTE_E4,
    NOTE_G4, NOTE_E4, NOTE_D4,
    NO_NOTE, NO_NOTE, NO_NOTE, NO_NOTE
  },

  // 第 9 組
  // 炊煙裊裊升起 隔江千萬里
  {
    NOTE_G3, NOTE_G3, NOTE_E4, NOTE_D4,
    NOTE_E4, NOTE_G4, NOTE_D4, NOTE_E4,
    NOTE_G4, NOTE_D4, NOTE_C4,
    NO_NOTE, NO_NOTE, NO_NOTE, NO_NOTE
  },

  // 第 10 組
  {
    NOTE_C4, NOTE_D4, NOTE_E4, NOTE_G4,
    NOTE_A4, NOTE_G4, NOTE_F4, NOTE_G4,
    NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4,
    NO_NOTE, NO_NOTE, NO_NOTE
  },

  // 第 11 組
  {
    NOTE_C4, NOTE_D4, NOTE_C4, NOTE_C4,
    NOTE_D4, NOTE_C4, NOTE_D4, NOTE_D4,
    NOTE_E4, NOTE_G4, NOTE_E4, NOTE_E4,
    NO_NOTE, NO_NOTE, NO_NOTE
  },

  // 第 12 組
  // 天青色等煙雨 而我在等你
  // 修正版：天、青 = G4，色 = E4
  {
    NOTE_G4, NOTE_G4, NOTE_E4, NOTE_D4,
    NOTE_E4, NOTE_A3, NOTE_D4, NOTE_E4,
    NOTE_G4, NOTE_E4, NOTE_D4,
    NO_NOTE, NO_NOTE, NO_NOTE, NO_NOTE
  }
};

// 每一組實際有幾個歌詞音
int groupLength[GROUP_COUNT] = {
  12,  // 第 1 組
  12,  // 第 2 組
  12,  // 第 3 組
  12,  // 第 4 組
  10,  // 第 5 組
  12,  // 第 6 組
  14,  // 第 7 組
  11,  // 第 8 組
  11,  // 第 9 組
  12,  // 第 10 組
  12,  // 第 11 組
  11   // 第 12 組
};

int currentGroup = 0;

void setup() {
  Serial.begin(9600);
  pinMode(buzzerPin, OUTPUT);

  Serial.println("4x4 Melody Keyboard Ready");
  Serial.println("Only lyric notes are kept.");
  Serial.println("D = Next Group");

  printCurrentGroup();
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    Serial.print("Pressed Key: ");
    Serial.println(key);

    // D 鍵切換下一組
    if (key == 'D') {
      nextGroup();
      return;
    }

    int keyIndex = getKeyIndex(key);

    if (keyIndex != -1) {
      playNote(currentGroup, keyIndex);
    }
  }
}

int getKeyIndex(char key) {
  for (int i = 0; i < 15; i++) {
    if (key == playKeys[i]) {
      return i;
    }
  }
  return -1;
}

void playNote(int group, int index) {
  // 超過該組歌詞音數量，不發聲
  if (index >= groupLength[group]) {
    Serial.println("No lyric note on this key.");
    noTone(buzzerPin);
    return;
  }

  int freq = melody[group][index];

  if (freq == NO_NOTE) {
    Serial.println("No note.");
    noTone(buzzerPin);
    return;
  }

  Serial.print("Group: ");
  Serial.print(group + 1);
  Serial.print(" | Note ");
  Serial.print(index + 1);
  Serial.print(" / ");
  Serial.print(groupLength[group]);
  Serial.print(" | Frequency: ");
  Serial.println(freq);

  tone(buzzerPin, freq, 300);
}

void nextGroup() {
  currentGroup++;

  if (currentGroup >= GROUP_COUNT) {
    currentGroup = 0;
  }

  printCurrentGroup();
}

void printCurrentGroup() {
  Serial.println("----------------------");
  Serial.print("Current Group: ");
  Serial.println(currentGroup + 1);

  Serial.print("This group has ");
  Serial.print(groupLength[currentGroup]);
  Serial.println(" lyric notes.");

  Serial.println("Play keys:");
  Serial.println("1 2 3 A 4 5 6 B 7 8 9 C * 0 #");
  Serial.println("D = Next Group");
  Serial.println("----------------------");
}