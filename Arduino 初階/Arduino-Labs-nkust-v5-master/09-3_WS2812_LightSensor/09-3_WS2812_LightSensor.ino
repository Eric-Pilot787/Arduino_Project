/*
  實驗 9-3：光線感應情境燈
  ------------------------------------------------
  單元九：WS2812 燈條

  實驗目標：
  結合光敏電阻（類比輸入）與 WS2812 燈條，
  依環境光線變化呈現不同的情境燈效果：
    環境明亮   -> 綠色
    環境昏暗   -> 橙黃色
    環境非常暗 -> 紅色閃爍（警示效果）

  接線：
  WS2812 燈條：DIN -> D6，5V -> 5V，GND -> GND （同實驗 9-1）
  光敏電阻模組：訊號腳位 (AO) -> A0，VCC -> 5V，GND -> GND
               （模組同單元六 實驗6-2，實際腳位請以模組標示為準）

  函式庫：
  Adafruit_NeoPixel
*/

#include <Adafruit_NeoPixel.h>

#define LED_PIN     6
#define LED_COUNT   8
#define LDR_PIN     A0    // 光敏電阻模組訊號腳位

// 亮度門檻值：請依教室實際亮度自行調整
// 建議先只上傳量測用的程式碼（見下方註解），
// 觀察不同亮度下 analogRead() 的讀值後再設定
int BRIGHT_THRESHOLD = 600;  // 高於此值視為「明亮」
int DARK_THRESHOLD   = 300;  // 低於此值視為「非常暗」

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
  strip.setBrightness(60);
  strip.show();

  Serial.begin(9600);   // 方便觀察光敏電阻讀值，設定門檻值時很有用
}

void loop() {
  int lightValue = analogRead(LDR_PIN);   // 讀取光敏電阻的亮度數值 (0~1023)
  Serial.println(lightValue);             // 序列埠印出讀值，方便調整門檻

  if (lightValue >= BRIGHT_THRESHOLD) {
    // 環境明亮 -> 綠色
    setAllPixels(0, 255, 0);
    strip.show();

  } else if (lightValue <= DARK_THRESHOLD) {
    // 環境非常暗 -> 紅色閃爍（警示效果）
    setAllPixels(255, 0, 0);
    strip.show();
    delay(200);
    strip.clear();
    strip.show();
    delay(200);

  } else {
    // 介於中間 -> 橙黃色（暖色提示燈）
    setAllPixels(255, 140, 0);
    strip.show();
  }

  delay(50);
}

// 將所有 LED 設定為同一個顏色（尚需呼叫 show() 才會生效）
void setAllPixels(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
}

/*
  延伸練習：
  1. 試著將顏色切換改為漸亮漸暗的呼吸燈效果，讓過渡更平滑
     （可用 for 迴圈搭配 setBrightness() 由小到大、再由大到小變化）
  2. 可嘗試結合單元八的蜂鳴器，當亮度過暗時加入聲音警示，
     做出更完整的情境警示裝置
*/
