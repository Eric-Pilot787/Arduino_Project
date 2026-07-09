/*
  實驗 9-1：點亮全彩 LED
  ------------------------------------------------
  單元九：WS2812 燈條

  實驗目標：
  將 8 位 WS2812 燈條的 8 顆 LED 分別設定為
  紅、橙、黃、綠、藍、靛、紫、白，如同彩虹排列

  接線：
  燈條 DIN -> Arduino D6
  燈條 5V  -> Arduino 5V
  燈條 GND -> Arduino GND

  函式庫：
  Adafruit_NeoPixel（請先透過「工具 > 管理程式庫」搜尋 NeoPixel 並安裝）
*/

#include <Adafruit_NeoPixel.h>

#define LED_PIN     6   // 燈條 DIN 所接的腳位
#define LED_COUNT   8   // 燈條上的 LED 顆數

// 建立 NeoPixel 物件
// NEO_GRB：本模組顏色資料排列順序為 G->R->B
// NEO_KHZ800：WS2812 / WS2812B 皆使用 800kHz 的訊號速率
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  delay(1000);
  strip.begin();       // 初始化燈條物件，設定資料腳位
  strip.setBrightness(60);  // 亮度調低一些，避免全白全亮電流過大 (0~255)
  strip.show();         // 一開始先送出「全部熄滅」的狀態

  // 依序設定 8 顆 LED 的顏色（彩虹排列）
  strip.setPixelColor(0, strip.Color(255,   0,   0));  // 紅
  strip.setPixelColor(1, strip.Color(255, 100,   0));  // 橙
  strip.setPixelColor(2, strip.Color(255, 255,   0));  // 黃
  strip.setPixelColor(3, strip.Color(  0, 255,   0));  // 綠
  strip.setPixelColor(4, strip.Color(  0,   0, 255));  // 藍
  strip.setPixelColor(5, strip.Color( 75,   0, 130));  // 靛
  strip.setPixelColor(6, strip.Color(128,   0, 128));  // 紫
  strip.setPixelColor(7, strip.Color(255, 255, 255));  // 白

  // 迴圈跑完後，只需呼叫一次 show()，就能一次把顏色送到全部 LED
  strip.show();
}

void loop() {
  // 本實驗只需要在 setup() 設定一次顏色即可，
  // loop() 保持空白即可維持顯示。
}

/*
  延伸練習：
  1. 試著修改每顆的顏色，或改變 setBrightness() 的數值，觀察亮度變化
  2. 在 loop() 中加入 delay() 並每次只點亮一顆（其餘熄滅），
     做出一顆一顆依序點亮的效果，例如：

     void loop() {
       for (int i = 0; i < LED_COUNT; i++) {
         strip.clear();
         strip.setPixelColor(i, strip.Color(255, 0, 0));
         strip.show();
         delay(150);
       }
     }
*/
