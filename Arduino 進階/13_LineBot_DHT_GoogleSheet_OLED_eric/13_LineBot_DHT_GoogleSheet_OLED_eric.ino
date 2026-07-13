#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <SimpleDHT.h>

// ===================== WiFi =====================
char ssid[] = "C220MIS";
char password[] = "misc220c220";

// ===================== DHT11 =====================
int pinDHT11 = 23;
SimpleDHT11 dht11(pinDHT11);

// ===================== Google Sheet =====================
String sheetId = "17ERcJt5D5JPdYbTf_O-vzoS4P5AbMoTG9IIm6l8-ERU";
String sheetName = "data1";
String googleScript = "https://script.google.com/macros/s/AKfycbyR-Yp-uu4nIvnjvnkILaQ5AX8yFxp-UpBO-Sqs0su3ai1N_BvQsz_Q/exec";

// ===================== LINE =====================
// 你提供的參數
String ChannelAccessToken = "dlU/OIvrEiprugIyfKm44lSFh79p54E0B36lAmwunkg+h4FVHAjgmIfuCyplIhWfPxki7WqzUlGnyxnMcfthzSjhxSZLkqwXPeIa7iJVwPn6lKEZouNN08k8d8rekHJn7rnwFqU8KpKFZ93rcm9Q7wdB04t89/1O/w1cDnyilFU=";
String LineUserId = "Uafc13fd6c6eec9294ac22547e7b85380";

// ===================== OLED =====================
#define SDA_PIN 21
#define SCL_PIN 22
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// ===================== LED =====================
const int greenLedPin = 2;
const int redLedPin = 4;

// ===================== 門檻 =====================
const int TEMP_LIMIT = 28;
const int HUM_LIMIT = 70;

// ===================== 週期 =====================
const unsigned long oledInterval = 5000;     // 5 秒
const unsigned long uploadInterval = 30000;  // 30 秒

unsigned long lastOLEDTime = 0;
unsigned long lastUploadTime = 0;

// ===================== 感測值 =====================
byte temperature = 0;
byte humidity = 0;
bool sensorOK = false;
bool abnormal = false;
bool lineAlertSent = false;  // 異常期間只通知一次

void setup() {
  Serial.begin(115200);

  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);

  Wire.begin(SDA_PIN, SCL_PIN);
  u8g2.begin();
  u8g2.enableUTF8Print();

  showBootScreen();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("開始連線到 WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nWiFi 連線完成");

  showMessage("C220教室溫溼度", "WiFi已連線", "");
}

void loop() {
  unsigned long now = millis();

  // 每 5 秒讀取一次 + 更新 OLED + LED + 異常判斷
  if (now - lastOLEDTime >= oledInterval) {
    lastOLEDTime = now;

    readSensor();
    updateLED();
    updateOLED();
    handleLineAlert();
  }

  // 每 30 秒上傳一次 Google Sheet
  if (now - lastUploadTime >= uploadInterval) {
    lastUploadTime = now;

    if (sensorOK) {
      uploadToGoogleSheet();
    }
  }
}

void readSensor() {
  byte t = 0;
  byte h = 0;
  int err = dht11.read(&t, &h, NULL);

  if (err != SimpleDHTErrSuccess) {
    Serial.print("DHT11 讀取失敗，錯誤碼 = ");
    Serial.println(err);
    sensorOK = false;
    abnormal = true;
    return;
  }

  temperature = t;
  humidity = h;
  sensorOK = true;

  abnormal = ((int)temperature > TEMP_LIMIT || (int)humidity > HUM_LIMIT);

  Serial.print("溫度：");
  Serial.print((int)temperature);
  Serial.print(" C, 濕度：");
  Serial.print((int)humidity);
  Serial.println(" %");
}

void updateLED() {
  if (!sensorOK) {
    digitalWrite(greenLedPin, LOW);
    digitalWrite(redLedPin, HIGH);
    return;
  }

  if (abnormal) {
    digitalWrite(greenLedPin, LOW);
    digitalWrite(redLedPin, HIGH);
  } else {
    digitalWrite(greenLedPin, HIGH);
    digitalWrite(redLedPin, LOW);
  }
}

void updateOLED() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_unifont_t_chinese2);
  u8g2.drawUTF8(0, 14, "C220教室溫溼度");

  if (!sensorOK) {
    u8g2.drawUTF8(0, 36, "感測器讀取失敗");
    u8g2.drawUTF8(0, 58, "請檢查DHT11");
    u8g2.sendBuffer();
    return;
  }

  char tempStr[24];
  char humStr[24];
  sprintf(tempStr, "溫度：%d*C", (int)temperature);
  sprintf(humStr, "濕度：%d%%", (int)humidity);

  u8g2.drawUTF8(0, 36, tempStr);
  u8g2.drawUTF8(0, 58, humStr);

  u8g2.sendBuffer();
}

void handleLineAlert() {
  if (!sensorOK) {
    if (!lineAlertSent) {
      sendLineMessage("C220教室警報：DHT11讀取失敗，請檢查感測器。");
      lineAlertSent = true;
    }
    return;
  }

  if (abnormal) {
    if (!lineAlertSent) {
      String msg = "C220教室溫溼度異常！\n";
      msg += "溫度：" + String((int)temperature) + " C\n";
      msg += "濕度：" + String((int)humidity) + " %\n";
      msg += "條件：溫度>28 或 濕度>70";
      sendLineMessage(msg);
      lineAlertSent = true;
    }
  } else {
    lineAlertSent = false;
  }
}

void uploadToGoogleSheet() {
  String data = String(temperature) + "," + String(humidity);
  sendToGoogleSheets("1", URLEncode(data.c_str()));
  Serial.println("Google Sheet 上傳成功");
}

void sendLineMessage(String message) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("LINE 發送失敗：WiFi 未連線");
    return;
  }

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  http.begin(client, "https://api.line.me/v2/bot/message/push");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + ChannelAccessToken);

  String payload = "{";
  payload += "\"to\":\"" + LineUserId + "\",";
  payload += "\"messages\":[";
  payload += "{";
  payload += "\"type\":\"text\",";
  payload += "\"text\":\"" + jsonEscape(message) + "\"";
  payload += "}";
  payload += "]";
  payload += "}";

  int httpResponseCode = http.POST(payload);
  Serial.print("LINE HTTP 回應碼：");
  Serial.println(httpResponseCode);

  String response = http.getString();
  Serial.println(response);

  http.end();
}

String jsonEscape(String input) {
  input.replace("\\", "\\\\");
  input.replace("\"", "\\\"");
  input.replace("\n", "\\n");
  input.replace("\r", "");
  return input;
}

void showBootScreen() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_unifont_t_chinese2);
  u8g2.drawUTF8(0, 16, "C220教室溫溼度");
  u8g2.drawUTF8(0, 40, "系統啟動中...");
  u8g2.sendBuffer();
}

void showMessage(const char* line1, const char* line2, const char* line3) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_unifont_t_chinese2);
  u8g2.drawUTF8(0, 14, line1);
  u8g2.drawUTF8(0, 36, line2);
  u8g2.drawUTF8(0, 58, line3);
  u8g2.sendBuffer();
}

// Google Sheet 上傳
void sendToGoogleSheets(const String& dateInclude, const String& data) {
  static WiFiClientSecure sheetClient;
  sheetClient.setInsecure();
  const char* host = "script.google.com";

  if (sheetClient.connect(host, 443)) {
    const String url = String() + "https://" + host +
      "/macros/s/AKfycbyR-Yp-uu4nIvnjvnkILaQ5AX8yFxp-UpBO-Sqs0su3ai1N_BvQsz_Q/exec?type=insert&dateInclude=" +
      dateInclude + "&sheetId=" + sheetId +
      "&sheetTag=" + URLEncode(sheetName.c_str()) +
      "&data=" + data;

    sheetClient.println("GET " + url + " HTTP/1.1");
    sheetClient.println(String() + "Host: " + host);
    sheetClient.println("Accept: */*");
    sheetClient.println("Connection: close");
    sheetClient.println();
    sheetClient.stop();
  }
}

// URL Encode
String URLEncode(const char* msg) {
  const char* hex = "0123456789abcdef";
  String encodedMsg = "";

  while (*msg != '\0') {
    if (('a' <= *msg && *msg <= 'z') ||
        ('A' <= *msg && *msg <= 'Z') ||
        ('0' <= *msg && *msg <= '9')) {
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