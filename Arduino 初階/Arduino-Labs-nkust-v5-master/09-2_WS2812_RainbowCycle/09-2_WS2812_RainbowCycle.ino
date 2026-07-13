/*
  實驗 9-2：彩虹跑馬燈效果
  ------------------------------------------------
  單元九：WS2812 燈條

  實驗目標：
  接線與實驗 9-1 相同。撰寫程式，讓顏色如彩虹般隨時間平移，
  產生流動效果（常稱為 Rainbow Cycle）。
  使用自訂函式 Wheel()，將 0~255 的數值轉換為色環上的顏色。

  接線：
  燈條 DIN -> Arduino D6
  燈條 5V  -> Arduino 5V
  燈條 GND -> Arduino GND

  函式庫：
  Adafruit_NeoPixel
*/

#include <Adafruit_NeoPixel.h>

#define LED_PIN     6
#define LED_COUNT   8

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// 色環上的位置轉換函式（0~255 對應色環上的一圈顏色）
// 取自 Adafruit_NeoPixel 的 strandtest 範例，是業界常用的寫法
uint32_t Wheel(byte wheelPos) {
  wheelPos = 255 - wheelPos;
  if (wheelPos < 85) {
    return strip.Color(255 - wheelPos * 3, 0, wheelPos * 3);
  }
  if (wheelPos < 170) {
    wheelPos -= 85;
    return strip.Color(0, wheelPos * 3, 255 - wheelPos * 3);
  }
  wheelPos -= 170;
  return strip.Color(wheelPos * 3, 255 - wheelPos * 3, 0);
}

void setup() {
  strip.begin();
  strip.setBrightness(60);
  strip.show();
}

void loop() {
  // offset 控制整體顏色隨時間平移的位移量
  for (int offset = 0; offset < 256; offset++) {
    // 內層迴圈依序設定每顆 LED 的顏色
    for (int i = 0; i < LED_COUNT; i++) {
      int wheelPos = (i * 256 / LED_COUNT + offset) & 255;
      strip.setPixelColor(i, Wheel(wheelPos));
    }
    strip.show();     // 更新完所有顏色後，送出顯示
    delay(30);        // 調整此處數值可改變流動速度
  }
}

/*
  延伸練習：
  1. 調整 delay() 的時間，觀察流動速度的變化
  2. 較新版本的 Adafruit_NeoPixel 函式庫也內建 rainbow() 函式，
     可以試著查閱說明文件並比較用法：
       strip.rainbow(0);   // 直接產生彩虹效果，內部已處理位移與顏色計算
       strip.show();
*/
