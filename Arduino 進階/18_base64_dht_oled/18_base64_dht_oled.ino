#include "esp_camera.h"
#include <EEPROM.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "Base64.h"


#include <SimpleDHT.h>
#include <Wire.h>
#include <U8g2lib.h>


// ====================================================
// WiFi 設定
// ====================================================
char ssid[] = "TheLin";
char password[] = "aaaaaaaa";


// ====================================================
// MQTT 設定
// ====================================================
char* MQTTServer = "mqttgo.io";
int MQTTPort = 1883;
char* MQTTUser = "";
char* MQTTPassword = "";


// 溫濕度資料發布主題
char* MQTTPubTopicData = "thelin/class220/data";


// 圖片發布主題
char* MQTTTopicPic = "thelin/class220/pic";


// 指令訂閱主題
// PHOTO、GLEDON、GLEDOFF、YLEDON、YLEDOFF、RLEDON、RLEDOFF 都從這個主題接收
char* MQTTSubTopicCmd = "thelin/class220/led";


// 溫濕度發布間隔
long MQTTLastPublishTime = 0;
long MQTTPublishInterval = 10000;


// ====================================================
// DHT11 設定
// ====================================================
int pinDHT11 = 33;
SimpleDHT11 dht11(pinDHT11);


byte nowTemp = 0;
byte nowHumi = 0;


// ====================================================
// LED 設定
// ====================================================
int pinGLED = 15;
int pinYLED = 2;
int pinRLED = 4;


// ====================================================
// OLED 設定
// 注意：ESP32-CAM 相機會用到 GPIO21、GPIO22
// 所以 OLED 不建議使用預設 21、22
// 這裡改用 SDA=13、SCL=14
// ====================================================
int pinOLED_SDA = 13;
int pinOLED_SCL = 14;


U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(
  U8G2_R0,
  U8X8_PIN_NONE
);


String statusText = "等待指令";


// ====================================================
// Camera 設定
// ====================================================
int vFlip = 1;    // 1=上下翻轉
int hMirror = 0;  // 1=左右翻轉


sensor_t* s;


// ====================================================
// MQTT Client
// ====================================================
WiFiClient WifiClient;
PubSubClient MQTTClient(WifiClient);


// 收到 PHOTO 後設成 true，在 loop 裡面拍照
bool photoRequested = false;


// ====================================================
// Function 宣告
// ====================================================
void setupCam();
void WifiConnecte();
void MQTTConnecte();
void MQTTCallback(char* topic, byte* payload, unsigned int length);


void ReadDHT(byte* temperature, byte* humidity);
void ShowOLED();


void flushCameraBuffer();
String SendImageMQTT_Base64();


// ====================================================
// setup
// ====================================================
void setup() {
  Serial.begin(115200);


  // LED 初始化
  pinMode(pinGLED, OUTPUT);
  pinMode(pinYLED, OUTPUT);
  pinMode(pinRLED, OUTPUT);


  digitalWrite(pinGLED, LOW);
  digitalWrite(pinYLED, LOW);
  digitalWrite(pinRLED, LOW);


  // OLED 初始化
  Wire.begin(pinOLED_SDA, pinOLED_SCL);


  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_unifont_t_chinese1);
  u8g2.setFontPosTop();


  ShowOLED();


  // Camera 初始化
  setupCam();


  // WiFi / MQTT 初始化
  WifiConnecte();


  MQTTClient.setServer(MQTTServer, MQTTPort);
  MQTTClient.setCallback(MQTTCallback);


  MQTTConnecte();


  statusText = "系統啟動完成";
  ShowOLED();


  // 開機後可以比較快第一次送出溫濕度
  MQTTLastPublishTime = millis() - MQTTPublishInterval;
}


// ====================================================
// loop
// ====================================================
void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    WifiConnecte();
  }


  if (!MQTTClient.connected()) {
    MQTTConnecte();
  }


  MQTTClient.loop();


  // 收到 PHOTO 指令後才拍照
  if (photoRequested) {
    photoRequested = false;


    statusText = "拍照中...";
    ShowOLED();


    Serial.println("收到 PHOTO 指令，準備清除舊畫面");
    flushCameraBuffer();


    Serial.println("開始拍照並上傳 MQTT");
    String result = SendImageMQTT_Base64();


    Serial.println(result);


    if (result == "MQTT 傳輸成功") {
      statusText = "拍照上傳完成";
    } else {
      statusText = "拍照上傳失敗";
    }


    ShowOLED();
  }


  // 每隔一段時間上傳溫濕度
  if ((millis() - MQTTLastPublishTime) >= MQTTPublishInterval) {
    ReadDHT(&nowTemp, &nowHumi);


    String data =
      "{\"temp\":" + String(nowTemp) +
      ",\"humi\":" + String(nowHumi) + "}";


    MQTTClient.publish(MQTTPubTopicData, data.c_str());


    Serial.print("溫濕度已推播到 MQTT Broker: ");
    Serial.println(data);


    ShowOLED();


    MQTTLastPublishTime = millis();
  }


  delay(100);
}


// ====================================================
// OLED 顯示
// ====================================================
void ShowOLED() {
  u8g2.clearBuffer();


  u8g2.setCursor(0, 5);
  u8g2.print(" 高雄科技大學");


  u8g2.setCursor(0, 25);
  u8g2.print("溫:");
  u8g2.print(nowTemp);
  u8g2.print("C 濕:");
  u8g2.print(nowHumi);
  u8g2.print("%");


  u8g2.setCursor(0, 45);
  u8g2.print(statusText);


  u8g2.sendBuffer();
}


// ====================================================
// 讀取 DHT11
// ====================================================
void ReadDHT(byte* temperature, byte* humidity) {
  int err = SimpleDHTErrSuccess;


  if ((err = dht11.read(temperature, humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("DHT11 讀取失敗, 錯誤 = ");
    Serial.print(SimpleDHTErrCode(err));
    Serial.print(",");
    Serial.println(SimpleDHTErrDuration(err));


    statusText = "DHT讀取失敗";
    return;
  }


  Serial.print("DHT11 讀取成功: ");
  Serial.print((int)*temperature);
  Serial.print("°C , ");
  Serial.print((int)*humidity);
  Serial.println("%");
}


// ====================================================
// WiFi 連線
// ====================================================
void WifiConnecte() {
  Serial.print("開始連線到 WiFi: ");
  Serial.println(ssid);


  WiFi.begin(ssid, password);


  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }


  Serial.println();
  Serial.println("WiFi 連線成功");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());


  statusText = "WiFi已連線";
  ShowOLED();
}


// ====================================================
// MQTT 連線
// ====================================================
void MQTTConnecte() {
  MQTTClient.setServer(MQTTServer, MQTTPort);
  MQTTClient.setCallback(MQTTCallback);


  while (!MQTTClient.connected()) {
    String MQTTClientid = "esp32-" + String(random(1000000, 9999999));


    if (MQTTClient.connect(MQTTClientid.c_str(), MQTTUser, MQTTPassword)) {
      Serial.println("MQTT 已連線");


      if (MQTTClient.subscribe(MQTTSubTopicCmd)) {
        Serial.print("已訂閱主題: ");
        Serial.println(MQTTSubTopicCmd);
      } else {
        Serial.println("MQTT 訂閱失敗");
      }


      statusText = "MQTT已連線";
      ShowOLED();


    } else {
      Serial.print("MQTT 連線失敗, 狀態碼 = ");
      Serial.println(MQTTClient.state());
      Serial.println("5 秒後重新連線");


      statusText = "MQTT連線失敗";
      ShowOLED();


      delay(5000);
    }
  }
}


// ====================================================
// 收到 MQTT 訊息
// ====================================================
void MQTTCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("收到主題: ");
  Serial.print(topic);
  Serial.print(" -> ");


  String payloadString = "";


  for (int i = 0; i < length; i++) {
    payloadString += (char)payload[i];
  }


  payloadString.trim();


  Serial.println(payloadString);


  if (strcmp(topic, MQTTSubTopicCmd) == 0) {
    // 拍照指令
    if (payloadString == "PHOTO") {
      Serial.println("收到 PHOTO 指令，準備拍照");
      statusText = "收到拍照指令";
      photoRequested = true;
    }


    // 綠燈，電燈
    else if (payloadString == "GLEDON") {
      digitalWrite(pinGLED, HIGH);
      statusText = "電燈已開啟";
      Serial.println(statusText);
    } else if (payloadString == "GLEDOFF") {
      digitalWrite(pinGLED, LOW);
      statusText = "電燈已關閉";
      Serial.println(statusText);
    }


    // 黃燈，除濕機
    else if (payloadString == "YLEDON") {
      digitalWrite(pinYLED, HIGH);
      statusText = "除濕機已開啟";
      Serial.println(statusText);
    } else if (payloadString == "YLEDOFF") {
      digitalWrite(pinYLED, LOW);
      statusText = "除濕機已關閉";
      Serial.println(statusText);
    }


    // 紅燈，風扇
    else if (payloadString == "RLEDON") {
      digitalWrite(pinRLED, HIGH);
      statusText = "風扇已開啟";
      Serial.println(statusText);
    } else if (payloadString == "RLEDOFF") {
      digitalWrite(pinRLED, LOW);
      statusText = "風扇已關閉";
      Serial.println(statusText);
    }


    // 其他文字直接顯示在 OLED
    else {
      statusText = payloadString;
      Serial.println(statusText);
    }


    ShowOLED();
  }
}


// ====================================================
// 清除相機舊畫面，避免拍到幾秒前的照片
// ====================================================
void flushCameraBuffer() {
  for (int i = 0; i < 3; i++) {
    camera_fb_t* fb = esp_camera_fb_get();


    if (fb) {
      esp_camera_fb_return(fb);
      Serial.println("已丟棄一張舊畫面");
    } else {
      Serial.println("清除舊畫面時取得影像失敗");
    }


    delay(100);
  }
}


// ====================================================
// 拍照並用 Base64 傳送到 MQTT
// ====================================================
String SendImageMQTT_Base64() {
  camera_fb_t* fb = esp_camera_fb_get();


  if (!fb) {
    Serial.println("Camera capture failed");
    return "Camera capture failed";
  }


  const char* prefix = "data:image/jpeg;base64,";
  int prefixLen = strlen(prefix);


  int encodedLen = base64_enc_len(fb->len);
  int totalLen = prefixLen + encodedLen;


  // 第三個參數 false：不要使用保留訊息 retained message
  boolean beginOK = MQTTClient.beginPublish(MQTTTopicPic, totalLen, false);


  if (!beginOK) {
    esp_camera_fb_return(fb);
    return "MQTT beginPublish 失敗";
  }


  // 先送出 data:image/jpeg;base64,
  MQTTClient.write((uint8_t*)prefix, prefixLen);


  // 每次處理 768 bytes
  // 768 是 3 的倍數，Base64 後會變成 1024 bytes
  const size_t rawChunkSize = 768;
  char output[1025];


  for (size_t n = 0; n < fb->len; n += rawChunkSize) {
    size_t rawLen;


    if (n + rawChunkSize < fb->len) {
      rawLen = rawChunkSize;
    } else {
      rawLen = fb->len - n;
    }


    int outLen = base64_enc_len(rawLen);


    base64_encode(output, (char*)(fb->buf + n), rawLen);


    MQTTClient.write((uint8_t*)output, outLen);


    delay(1);
  }


  boolean isPublished = MQTTClient.endPublish();


  esp_camera_fb_return(fb);


  if (isPublished) {
    return "MQTT 傳輸成功";
  } else {
    return "MQTT 傳輸失敗，請檢查網路設定";
  }
}


// ====================================================
// Camera 初始化
// ====================================================
void setupCam() {
  camera_config_t config;


  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;


  config.pin_d0 = 5;
  config.pin_d1 = 18;
  config.pin_d2 = 19;
  config.pin_d3 = 21;
  config.pin_d4 = 36;
  config.pin_d5 = 39;
  config.pin_d6 = 34;
  config.pin_d7 = 35;


  config.pin_xclk = 0;
  config.pin_pclk = 22;
  config.pin_vsync = 25;
  config.pin_href = 23;


  config.pin_sscb_sda = 26;
  config.pin_sscb_scl = 27;


  config.pin_pwdn = 32;
  config.pin_reset = -1;


  config.xclk_freq_hz = 5000000;
  config.pixel_format = PIXFORMAT_JPEG;


  config.frame_size = FRAMESIZE_UXGA;
  config.jpeg_quality = 10;


  // 重點：改成 1，減少拿到舊畫面的機率
  config.fb_count = 1;


  esp_err_t err = esp_camera_init(&config);


  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    statusText = "Camera失敗";
    ShowOLED();
    return;
  }


  s = esp_camera_sensor_get();


  s->set_brightness(s, 1);
  s->set_contrast(s, 1);
  s->set_saturation(s, 1);


  s->set_pixformat(s, PIXFORMAT_JPEG);
  s->set_vflip(s, vFlip);
  s->set_hmirror(s, hMirror);


  // 建議先用 XGA，如果 MQTT 傳輸太慢或失敗，可以改成 VGA 或 QVGA
  s->set_framesize(s, FRAMESIZE_VGA);


  Serial.println("Camera Setup OK");
}

