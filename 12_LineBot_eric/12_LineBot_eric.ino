#include <WiFi.h>
#include <WiFiClientSecure.h>
WiFiClientSecure client;
//Wifi設定
char ssid[] = "C220MIS";          // WiFi名稱
char password[] = "misc220c220";  // WiFi密碼

// Line設定值
String ChannelAccessToken = "dlU/OIvrEiprugIyfKm44lSFh79p54E0B36lAmwunkg+h4FVHAjgmIfuCyplIhWfPxki7WqzUlGnyxnMcfthzSjhxSZLkqwXPeIa7iJVwPn6lKEZouNN08k8d8rekHJn7rnwFqU8KpKFZ93rcm9Q7wdB04t89/1O/w1cDnyilFU=";
String LineUserId = "Uafc13fd6c6eec9294ac22547e7b85380";
#include <SimpleDHT.h>
//------ 以下修改成你腳位 ------
int pinDHT11 = 23;  //DHT11
SimpleDHT11 dht11(pinDHT11);
void setup() {
  Serial.begin(115200);
  WifiConnecte();
}
void loop() {
  //讀取溫濕度
  byte temperature = 0;
  byte humidity = 0;
  ReadDHT(&temperature, &humidity);  //*=>指位器 &=>取位器,位=>記憶體
  if (temperature >= 24 or humidity >= 60) {
    String msg = "溫溼度已超過警戒,溫度:" + String(temperature) + " C,濕度:" + String(humidity) + " %";
    // 用Line串訊息 文字 貼圖包 貼圖編號
    sendLineBotMsg(msg, 6370, 11088016);
  } else {
    Serial.println("溫溼度正常不發送警報");
  }
  delay(2000);
}

//開始WiFi連線
void WifiConnecte() {
  //開始WiFi連線

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi連線成功");
  Serial.print("IP Address:");
  Serial.println(WiFi.localIP());
}

void sendLineBotMsg(String myMsg, int stkPkgId, int stkId) {
  static WiFiClientSecure line_client;
  line_client.setInsecure();
  myMsg = "{\"to\":\"" + LineUserId + "\",\"messages\":[{\"type\":\"text\",\"text\":\"" + myMsg + "\"}";
  if (stkPkgId > 0 && stkId > 0)
    myMsg += ",{\"type\":\"sticker\",\"packageId\":\"" + String(stkPkgId) + "\",\"stickerId\":\"" + String(stkId) + "\"}";
  myMsg += "]}";
  Serial.println(myMsg);
  if (line_client.connect("api.line.me", 443)) {
    line_client.println("POST /v2/bot/message/push HTTP/1.1");
    line_client.println("Connection: close");
    line_client.println("Host: api.line.me");
    line_client.println("Authorization: Bearer " + ChannelAccessToken);
    line_client.println("Content-Type: application/json; charset=utf-8");
    line_client.println("Content-Length: " + String(myMsg.length()));
    line_client.println();
    line_client.println(myMsg);
    line_client.println();
    line_client.stop();
  } else {
    Serial.println("Line Bot push failed");
  }
}
//讀取DHT11溫濕度
void ReadDHT(byte *temperature, byte *humidity) {
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(temperature, humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("讀取失敗,錯誤訊息=");
    Serial.print(SimpleDHTErrCode(err));
    Serial.print(",");

    Serial.println(SimpleDHTErrDuration(err));
    delay(1000);
    return;
  }
  Serial.print("DHT讀取成功:");
  Serial.print((int)*temperature);
  Serial.print(" *C, ");
  Serial.print((int)*humidity);
  Serial.println(" H");
}

