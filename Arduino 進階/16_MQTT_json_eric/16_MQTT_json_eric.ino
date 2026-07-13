//MQTT互動+JSON資料格式+注意事項顯示在OLED上
#include <WiFi.h>
#include <PubSubClient.h>
#include <SimpleDHT.h>
#include <Wire.h>
#include <U8g2lib.h>


//================ OLED ===================
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);


//OLED要顯示的資料
byte nowTemp = 0;
byte nowHumi = 0;
String statusText = "等待指令";


//---------------- WiFi ----------------
char ssid[] = "C220MIS";
char password[] = "misc220c220";


//---------------- DHT11 ----------------
int pinDHT11 = 17;
SimpleDHT11 dht11(pinDHT11);


//---------------- LED ----------------
int pinGLED = 15;
int pinYLED = 2;
int pinRLED = 4;


//---------------- MQTT ----------------
char* MQTTServer = "mqttgo.io";
int MQTTPort = 1883;
char* MQTTUser = "";
char* MQTTPassword = "";


//發布Topic
char* MQTTPubTopic1 = "thelin/class220/data";


//訂閱Topic
char* MQTTSubTopic1 = "thelin/class220/led";


long MQTTLastPublishTime;
long MQTTPublishInterval = 10000;


WiFiClient WifiClient;
PubSubClient MQTTClient(WifiClient);


//================ Function =================
void WifiConnecte();
void MQTTConnecte();
void MQTTCallback(char* topic, byte* payload, unsigned int length);
void ReadDHT(byte* temperature, byte* humidity);
void ShowOLED();


//====================================================
void setup() {
  Serial.begin(115200);


  pinMode(pinGLED, OUTPUT);
  pinMode(pinYLED, OUTPUT);
  pinMode(pinRLED, OUTPUT);


  digitalWrite(pinGLED, LOW);
  digitalWrite(pinYLED, LOW);
  digitalWrite(pinRLED, LOW);


  //OLED初始化
  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_unifont_t_chinese1);
  u8g2.setFontPosTop();


  ShowOLED();


  WifiConnecte();
  MQTTConnecte();
}


//====================================================
void loop() {
  if (WiFi.status() != WL_CONNECTED)
    WifiConnecte();


  if (!MQTTClient.connected())
    MQTTConnecte();


  if ((millis() - MQTTLastPublishTime) >= MQTTPublishInterval) {
    ReadDHT(&nowTemp, &nowHumi);


    String data =
      "{\"temp\":" + String(nowTemp) + ",\"humi\":" + String(nowHumi) + "}";


    MQTTClient.publish(MQTTPubTopic1, data.c_str());


    Serial.println("溫濕度已推播到MQTT Broker");


    ShowOLED();


    MQTTLastPublishTime = millis();
  }


  MQTTClient.loop();


  delay(100);
}


//====================================================
// OLED顯示
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


//====================================================
//讀取DHT11
void ReadDHT(byte* temperature, byte* humidity) {
  int err = SimpleDHTErrSuccess;


  if ((err = dht11.read(temperature, humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("讀取失敗,錯誤=");
    Serial.print(SimpleDHTErrCode(err));
    Serial.print(",");
    Serial.println(SimpleDHTErrDuration(err));
    delay(1000);
    return;
  }


  Serial.print("DHT讀取成功：");
  Serial.print((int)*temperature);
  Serial.print("°C , ");
  Serial.print((int)*humidity);
  Serial.println("%");
}


//====================================================
//WiFi連線
void WifiConnecte() {
  WiFi.begin(ssid, password);


  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }


  Serial.println();
  Serial.println("WiFi連線成功");
  Serial.print("IP:");
  Serial.println(WiFi.localIP());
}


//====================================================
//MQTT連線
void MQTTConnecte() {
  MQTTClient.setServer(MQTTServer, MQTTPort);
  MQTTClient.setCallback(MQTTCallback);


  while (!MQTTClient.connected()) {
    String MQTTClientid = "esp32-" + String(random(1000000, 9999999));


    if (MQTTClient.connect(MQTTClientid.c_str(), MQTTUser, MQTTPassword)) {
      Serial.println("MQTT已連線");
      MQTTClient.subscribe(MQTTSubTopic1);
    } else {
      Serial.print("MQTT連線失敗,狀態碼=");
      Serial.println(MQTTClient.state());
      Serial.println("5秒後重新連線");
      delay(5000);
    }
  }
}


//====================================================
//收到MQTT訊息
void MQTTCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print(topic);
  Serial.print(" -> ");


  String payloadString = "";


  for (int i = 0; i < length; i++) {
    payloadString += (char)payload[i];
  }


  Serial.println(payloadString);


  if (strcmp(topic, MQTTSubTopic1) == 0) {
    //========================
    // 綠燈(電燈)
    //========================
    if (payloadString == "GLEDON") {
      digitalWrite(pinGLED, HIGH);
      statusText = "電燈已開啟";
      Serial.println(statusText);
    } else if (payloadString == "GLEDOFF") {
      digitalWrite(pinGLED, LOW);
      statusText = "電燈已關閉";
      Serial.println(statusText);
    }


    //========================
    // 黃燈(除濕機)
    //========================
    else if (payloadString == "YLEDON") {
      digitalWrite(pinYLED, HIGH);
      statusText = "除濕機已開啟";
      Serial.println(statusText);
    } else if (payloadString == "YLEDOFF") {
      digitalWrite(pinYLED, LOW);
      statusText = "除濕機已關閉";
      Serial.println(statusText);
    }


    //========================
    // 紅燈(風扇)
    //========================
    else if (payloadString == "RLEDON") {
      digitalWrite(pinRLED, HIGH);
      statusText = "風扇已開啟";
      Serial.println(statusText);
    } else if (payloadString == "RLEDOFF") {
      digitalWrite(pinRLED, LOW);
      statusText = "風扇已關閉";
      Serial.println(statusText);
    } else {
      statusText = payloadString;  //直接把收到的文字顯示在OLED
      Serial.println(statusText);
    }


    //更新OLED
    ShowOLED();
  }
}

