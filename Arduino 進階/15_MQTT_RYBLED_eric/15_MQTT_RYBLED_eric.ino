#include <WiFi.h>
#include <PubSubClient.h>
#include <SimpleDHT.h>

//---------------- WiFi ----------------
char ssid[] = "C220MIS";
char password[] = "misc220c220";

//---------------- DHT11 ----------------
int pinDHT11 = 17;
SimpleDHT11 dht11(pinDHT11);

//---------------- RGB LED ----------------
int pinGLED = 2;   // 綠色通道 G
int pinYLED = 15;    // 這裡實際可視為藍色通道 B，但本題不用
int pinRLED = 4;    // 紅色通道 R

//---------------- MQTT ----------------
char* MQTTServer = "mqttgo.io";
int MQTTPort = 1883;
char* MQTTUser = "";
char* MQTTPassword = "";

//發布Topic
char* MQTTPubTopic1 = "thelin/class220/temp";
char* MQTTPubTopic2 = "thelin/class220/humi";

//訂閱Topic
char* MQTTSubTopic1 = "thelin/class220/led";

long MQTTLastPublishTime;
long MQTTPublishInterval = 10000;

WiFiClient WifiClient;
PubSubClient MQTTClient(WifiClient);

//---------------- Function Prototype ----------------
void WifiConnecte();
void MQTTConnecte();
void MQTTCallback(char* topic, byte* payload, unsigned int length);
void ReadDHT(byte *temperature, byte *humidity);

void allLedOff();
void showRed();
void showGreen();
void showYellow();

//====================================================
void setup() {
  Serial.begin(115200);

  pinMode(pinGLED, OUTPUT);
  pinMode(pinYLED, OUTPUT);
  pinMode(pinRLED, OUTPUT);

  allLedOff();

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
    byte temperature = 0;
    byte humidity = 0;

    ReadDHT(&temperature, &humidity);

    MQTTClient.publish(MQTTPubTopic1, String(temperature).c_str());
    MQTTClient.publish(MQTTPubTopic2, String(humidity).c_str());

    Serial.println("溫溼度已推播到MQTT Broker");

    MQTTLastPublishTime = millis();
  }

  MQTTClient.loop();
  delay(100);
}

//====================================================
//讀取DHT11
void ReadDHT(byte *temperature, byte *humidity) {
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
  Serial.print(" °C , ");
  Serial.print((int)*humidity);
  Serial.println(" %");
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
    // 綠燈
    //========================
    if (payloadString == "GLEDON") {
      showGreen();
      Serial.println("綠燈開");
    }
    else if (payloadString == "GLEDOFF") {
      allLedOff();
      Serial.println("綠燈關");
    }

    //========================
    // 黃燈（RGB 混色）
    //========================
    else if (payloadString == "YLEDON") {
      showYellow();
      Serial.println("黃燈開");
    }
    else if (payloadString == "YLEDOFF") {
      allLedOff();
      Serial.println("黃燈關");
    }

    //========================
    // 紅燈
    //========================
    else if (payloadString == "RLEDON") {
      showRed();
      Serial.println("紅燈開");
    }
    else if (payloadString == "RLEDOFF") {
      allLedOff();
      Serial.println("紅燈關");
    }

    else {
      Serial.println("未知指令！");
    }
  }
}

//====================================================
// 關閉所有顏色
void allLedOff() {
  analogWrite(pinRLED, 0);
  analogWrite(pinGLED, 0);
  analogWrite(pinYLED, 0);
}

//====================================================
// 顯示紅色
void showRed() {
  analogWrite(pinRLED, 255);
  analogWrite(pinGLED, 0);
  analogWrite(pinYLED, 0);
}

//====================================================
// 顯示綠色
void showGreen() {
  analogWrite(pinRLED, 0);
  analogWrite(pinGLED, 255);
  analogWrite(pinYLED, 0);
}

//====================================================
// 顯示黃色 = 紅 + 綠
void showYellow() {
  analogWrite(pinRLED, 255);
  analogWrite(pinGLED, 255);   // 可調亮度，讓黃色更順眼
  analogWrite(pinYLED, 0);
}