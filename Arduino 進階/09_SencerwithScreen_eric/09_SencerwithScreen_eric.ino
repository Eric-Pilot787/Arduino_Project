#include <Wire.h>
#include <U8g2lib.h>
#include <SimpleDHT.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

int pinDHT11 = 23;
SimpleDHT11 dht11(pinDHT11);

void setup() {
  Serial.begin(115200);
  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_unifont_t_chinese1);
  u8g2.setFontPosTop();
}

void loop() {
  byte temperature = 0;
  byte humidity = 0;
  int err = dht11.read(&temperature, &humidity, NULL);

  u8g2.clearBuffer();

  if (err != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err=");
    Serial.println(err);

    u8g2.setCursor(0, 5);
    u8g2.print("DHT11讀取失敗");
    u8g2.sendBuffer();
    delay(1000);
    return;
  }

  Serial.print("Temperature: ");
  Serial.print((int)temperature);
  Serial.print(" *C, Humidity: ");
  Serial.print((int)humidity);
  Serial.println(" H");

  u8g2.setCursor(0, 5);
  u8g2.print("高雄科技大學");

  u8g2.setCursor(0, 25);
  u8g2.print("溫度：");
  u8g2.print((int)temperature);
  u8g2.print(" *C");

  u8g2.setCursor(0, 45);
  u8g2.print("濕度：");
  u8g2.print((int)humidity);
  u8g2.print(" %");

  u8g2.drawLine(0, 11, 120, 11);
  u8g2.sendBuffer();

  delay(1500);
}