#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <U8g2lib.h>

const char* ssid = "C220MIS";
const char* password = "misc220c220";

const char* url =
  "https://data.moenv.gov.tw/api/v2/aqx_p_02?format=json&limit=100&api_key=6b143ef9-f251-43e4-add6-a5557c7ffa1c";

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

#define RGB_R 4
#define RGB_G 2
#define RGB_B 15

#define CH_R 0
#define CH_G 1
#define CH_B 2

void setRGB(int r, int g, int b) {
  ledcWrite(CH_R, r);
  ledcWrite(CH_G, g);
  ledcWrite(CH_B, b);
}

void setup() {
  Serial.begin(115200);

  ledcSetup(CH_R, 5000, 8);
  ledcSetup(CH_G, 5000, 8);
  ledcSetup(CH_B, 5000, 8);

  ledcAttachPin(RGB_R, CH_R);
  ledcAttachPin(RGB_G, CH_G);
  ledcAttachPin(RGB_B, CH_B);
  setRGB(0, 0, 0);

  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_unifont_t_chinese1);
  u8g2.setFontPosTop();

  u8g2.clearBuffer();
  u8g2.setCursor(0, 20);
  u8g2.print("WiFi連線中...");
  u8g2.sendBuffer();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("連線中");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println("WiFi連線成功");

  u8g2.clearBuffer();
  u8g2.setCursor(0, 20);
  u8g2.print("WiFi連線成功");
  u8g2.sendBuffer();

  delay(1000);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi斷線");
    delay(3000);
    return;
  }

  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    DynamicJsonDocument doc(65536);
    DeserializationError error = deserializeJson(doc, payload);

    if (!error) {
      JsonArray records = doc.as<JsonArray>();

      int pm25 = -1;
      String updateTime = "";

      for (JsonObject obj : records) {
        String site = obj["site"].as<String>();
        if (site == "楠梓") {
          pm25 = obj["pm25"].as<String>().toInt();
          updateTime = obj["datacreationdate"].as<String>();
          break;
        }
      }

      if (pm25 == -1) {
        u8g2.clearBuffer();
        u8g2.setCursor(0, 20);
        u8g2.print("找不到楠梓資料");
        u8g2.sendBuffer();
        setRGB(0, 0, 0);
      } else {
        String level;
        if (pm25 < 10) {
          setRGB(0, 255, 0);
          level = "安全";
        } else if (pm25 <= 20) {
          setRGB(255, 255, 0);
          level = "注意";
        } else {
          setRGB(255, 0, 0);
          level = "危險";
        }

        u8g2.clearBuffer();
        u8g2.setCursor(0, 0);
        u8g2.print("楠梓區 PM2.5");
        u8g2.setCursor(0, 18);
        u8g2.print("數值：");
        u8g2.print(pm25);
        u8g2.print(" ug/m3");
        u8g2.setCursor(0, 36);
        u8g2.print("狀態：");
        u8g2.print(level);
        u8g2.setCursor(0, 54);
        u8g2.print("更新:");
        if (updateTime.length() >= 16) u8g2.print(updateTime.substring(11, 16));
        u8g2.sendBuffer();
      }
    }
  }

  http.end();
  delay(60000);
}