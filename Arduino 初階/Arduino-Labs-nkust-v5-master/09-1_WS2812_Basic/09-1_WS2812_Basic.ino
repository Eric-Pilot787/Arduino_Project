/*
  實驗 9-1 改寫：全彩 WS2812 燈條 - 溫和黃光模式
  ------------------------------------------------
  接線：
  燈條 DIN -> Arduino D6
  燈條 5V  -> Arduino 5V
  燈條 GND -> Arduino GND
*/

#include <Adafruit_NeoPixel.h>

#define LED_PIN       6   // 燈條 DIN 所接的腳位
#define LED_COUNT     8   // 燈條上的 LED 顆數

// 💡 【亮度調整欄位】範圍為 0 ~ 255 (建議 20~80，過高會較刺眼且耗電)
#define BRIGHTNESS   50  

// 建立 NeoPixel 物件
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  delay(1000);
  strip.begin();           // 初始化燈條物件
  strip.setBrightness(BRIGHTNESS); // 使用上方定義的欄位設定亮度
  strip.show();            // 一開始先送出「全部熄滅」

  // 設定溫和黃光 (Warm Yellow / Soft Gold) 的 RGB 數值
  // R: 255, G: 180, B: 50 可以營造出溫和、不刺眼的暖黃光
  uint32_t warmYellow = strip.Color(255, 180, 50);

  // 使用迴圈將 8 顆 LED 全部設定為溫和黃光
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, warmYellow);
  }

  // 將設定結果一次更新到燈條上
  strip.show();
}

void loop() {
  // setup() 設定完成後即可維持恆亮
}