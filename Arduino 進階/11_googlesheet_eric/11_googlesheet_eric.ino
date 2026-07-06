#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <SimpleDHT.h>

// 請修改以下參數--------------------------------------------
char ssid[] = "C220MIS";          
char password[] = "misc220c220";  

int pinDHT11 = 23;                
SimpleDHT11 dht11(pinDHT11);      

// 修改為你的 google sheetID 及工作表名稱
String sheetId = "17ERcJt5D5JPdYbTf_O-vzoS4P5AbMoTG9IIm6l8-ERU";
String sheetName = "data1";

// OLED 設定
#define SDA_PIN 21
#define SCL_PIN 22

// SSD1306 128x64 I2C OLED
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

void setup() {
  Serial.begin(115200);

  // 啟動 I2C 與 OLED
  Wire.begin(SDA_PIN, SCL_PIN);
  u8g2.begin();
  u8g2.enableUTF8Print();

  // 開機畫面
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_unifont_t_chinese2);
  u8g2.drawUTF8(0, 16, "C220教室溫溼度");
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawUTF8(0, 40, "System Start...");
  u8g2.sendBuffer();

  Serial.print("開始連線到無線網路SSID:");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("連線完成");
}

void loop() {
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;

  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("溫度計讀取失敗，錯誤碼=");
    Serial.println(err);

    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_unifont_t_chinese2);
    u8g2.drawUTF8(0, 16, "C220教室溫溼度");
    u8g2.drawUTF8(0, 40, "感測器讀取失敗");
    u8g2.sendBuffer();

    delay(1000);
    return;
  }

  // 序列埠顯示
  Serial.print("溫度計讀取成功: ");
  Serial.print((int)temperature);
  Serial.print(" *C, ");
  Serial.print((int)humidity);
  Serial.println(" H");

  // OLED 顯示
  showOLED(temperature, humidity);

  // 組成 CSV：欄位1,欄位2
  String data = String(temperature) + "," + String(humidity);

  // 上傳到 Google Sheet
  sendToGoogleSheets("1", URLEncode(data.c_str()));
  Serial.println("Google Sheet上傳成功");

  delay(10000);
}

void showOLED(byte temperature, byte humidity) {
  char tempStr[20];
  char humiStr[20];

  sprintf(tempStr, "溫度：%d*C", (int)temperature);
  sprintf(humiStr, "濕度：%d%%", (int)humidity);

  u8g2.clearBuffer();

  u8g2.setFont(u8g2_font_unifont_t_chinese2);
  u8g2.drawUTF8(0, 16, "C220教室溫溼度");

  u8g2.drawUTF8(0, 38, tempStr);
  u8g2.drawUTF8(0, 60, humiStr);

  u8g2.sendBuffer();
}

// google Sheet 上傳
void sendToGoogleSheets(const String& dateInclude, const String& data) {
  static WiFiClientSecure sheetClient;
  sheetClient.setInsecure();
  const char* host = "script.google.com";

  if (sheetClient.connect(host, 443)) {
    const String url = String() + "https://" + host +
      "/macros/s/AKfycbyR-Yp-uu4nIvnjvnkILaQ5AX8yFxp-UpBO-Sqs0su3ai1N_BvQsz_Q/exec?type=insert&dateInclude=" +
      dateInclude + "&sheetId=" + sheetId + "&sheetTag=" + URLEncode(sheetName.c_str()) + "&data=" + data;

    sheetClient.println("GET " + url + " HTTP/1.1");
    sheetClient.println(String() + "Host: " + host);
    sheetClient.println("Accept: */*");
    sheetClient.println("Connection: close");
    sheetClient.println();
    sheetClient.println();
    sheetClient.stop();
  }
}

// 網址編碼
String URLEncode(const char* msg) {
  const char* hex = "0123456789abcdef";
  String encodedMsg = "";

  while (*msg != '\0') {
    if (('a' <= *msg && *msg <= 'z')
        || ('A' <= *msg && *msg <= 'Z')
        || ('0' <= *msg && *msg <= '9')) {
      encodedMsg += *msg;
    } else {
      encodedMsg += '%';
      encodedMsg += hex[*msg >> 4];
      encodedMsg += hex[*msg & 15];
    }
    msg++;
  }
  return encodedMsg;
}