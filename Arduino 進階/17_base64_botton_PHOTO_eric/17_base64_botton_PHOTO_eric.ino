#include "esp_camera.h"
#include <EEPROM.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "Base64.h"


// ------ 以下修改成你的設定 ------
int vFlip = 1;    // 1=上下翻轉
int hMirror = 0;  // 1=左右翻轉


char* ssid = "TheLin";
char* password = "aaaaaaaa";


char* MQTTServer = "mqttgo.io";  // 免註冊 MQTT 伺服器
int MQTTPort = 1883;             // MQTT Port
char* MQTTUser = "";             // 不須帳密
char* MQTTPassword = "";         // 不須帳密


// 圖片上傳主題
char* MQTTTopicPic1 = "thelin/class220/pic";


// 拍照指令主題
char* MQTTTopicCmd = "thelin/class220/led";


// 收到 PHOTO 後設成 true，在 loop 裡面再拍照
bool photoRequested = false;


sensor_t* s;


WiFiClient WifiClient;
PubSubClient MQTTClient(WifiClient);


void setup() {
  Serial.begin(115200);


  setupCam();


  WiFiConnect();


  MQTTClient.setServer(MQTTServer, MQTTPort);
  MQTTClient.setCallback(MQTTCallback);
}


void loop() {
  // 如果 WiFi 連線中斷，則重啟 WiFi 連線
  if (WiFi.status() != WL_CONNECTED) {
    WiFiConnect();
  }


  // 如果 MQTT 連線中斷，則重啟 MQTT 連線
  if (!MQTTClient.connected()) {
    MQTTConnecte();
  }


  // 更新 MQTT 訂閱狀態
  MQTTClient.loop();


  // 收到 PHOTO 指令後才拍照
  if (photoRequested) {
    photoRequested = false;


    Serial.println("收到拍照要求，準備清除舊畫面...");
    flushCameraBuffer();


    Serial.println("開始拍照並上傳 MQTT...");
    String result = SendImageMQTT_Base64();


    Serial.println(result);
  }
}


// 連線到 WiFi
void WiFiConnect() {
  Serial.print("開始連線到: ");
  Serial.println(ssid);


  WiFi.begin(ssid, password);


  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }


  Serial.println("");
  Serial.print("WiFi connected, IP address: ");
  Serial.println(WiFi.localIP());
}


// 開始 MQTT 連線伺服器
void MQTTConnecte() {
  MQTTClient.setServer(MQTTServer, MQTTPort);
  MQTTClient.setCallback(MQTTCallback);


  while (!MQTTClient.connected()) {
    String MQTTClientid = "esp32-" + String(random(1000000, 9999999));


    if (MQTTClient.connect(MQTTClientid.c_str(), MQTTUser, MQTTPassword)) {
      Serial.println("MQTT 已連線");


      // 訂閱拍照指令主題
      if (MQTTClient.subscribe(MQTTTopicCmd)) {
        Serial.print("已訂閱主題: ");
        Serial.println(MQTTTopicCmd);
      } else {
        Serial.println("訂閱失敗");
      }


    } else {
      Serial.print("MQTT 連線失敗, 狀態碼 = ");
      Serial.println(MQTTClient.state());
      Serial.println("五秒後重新連線");
      delay(5000);
    }
  }
}


// 清除相機舊畫面，避免拿到幾秒前的照片
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


// 拍照傳送到 MQTT，Binary 版本，可保留不用
String SendImageMQTT() {
  camera_fb_t* fb = esp_camera_fb_get();


  if (!fb) {
    Serial.println("Camera capture failed");
    return "Camera capture failed";
  }


  size_t fbLen = fb->len;


  // 第三個參數 false：不要保留舊照片
  MQTTClient.beginPublish(MQTTTopicPic1, fbLen, false);


  uint8_t* fbBuf = fb->buf;


  for (size_t n = 0; n < fbLen; n = n + 2048) {
    if (n + 2048 < fbLen) {
      MQTTClient.write(fbBuf, 2048);
      fbBuf += 2048;
    } else {
      size_t remainder = fbLen - n;
      MQTTClient.write(fbBuf, remainder);
    }


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


// 拍照傳送到 MQTT Base64
String SendImageMQTT_Base64() {
  camera_fb_t* fb = esp_camera_fb_get();


  if (!fb) {
    Serial.println("Camera capture failed");
    return "Camera capture failed";
  }


  const char* prefix = "data:image/jpeg;base64,";
  int prefixLen = strlen(prefix);


  // Base64 後的總長度
  int encodedLen = base64_enc_len(fb->len);
  int totalLen = prefixLen + encodedLen;


  // 第三個參數 false：不要保留舊照片
  boolean beginOK = MQTTClient.beginPublish(MQTTTopicPic1, totalLen, false);


  if (!beginOK) {
    esp_camera_fb_return(fb);
    return "MQTT beginPublish 失敗";
  }


  // 先送出 data:image/jpeg;base64,
  MQTTClient.write((uint8_t*)prefix, prefixLen);


  // 每次處理 768 bytes
  // 768 是 3 的倍數，Base64 編碼後剛好變成 1024 bytes
  const size_t rawChunkSize = 768;


  // 768 bytes 轉 Base64 會變成 1024 字元，再加 1 個結尾字元
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


// 接收到訂閱時
void MQTTCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("收到主題: ");
  Serial.println(topic);


  String payloadString = "";


  for (int i = 0; i < length; i++) {
    payloadString += (char)payload[i];
  }


  payloadString.trim();


  Serial.print("收到內容: ");
  Serial.println(payloadString);


  // 判斷是否為拍照指令
  if (String(topic) == MQTTTopicCmd && payloadString == "PHOTO") {
    Serial.println("收到 PHOTO 指令，準備拍照");
    photoRequested = true;
  }
}


// 鏡頭設定
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


  // 重點：改成 1，避免雙緩衝拿到舊畫面
  config.fb_count = 1;


  esp_err_t err = esp_camera_init(&config);


  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    return;
  }


  s = esp_camera_sensor_get();


  s->set_brightness(s, 1);  // 亮度 -2 to 2
  s->set_contrast(s, 1);    // 對比 -2 to 2
  s->set_saturation(s, 1);  // 飽和 -2 to 2


  s->set_pixformat(s, PIXFORMAT_JPEG);
  s->set_vflip(s, vFlip);
  s->set_hmirror(s, hMirror);


  // 建議先用 XGA，如果 MQTT 傳輸失敗，可改成 VGA 或 QVGA
  s->set_framesize(s, FRAMESIZE_VGA);


  Serial.println("Camera Setup OK");
}

