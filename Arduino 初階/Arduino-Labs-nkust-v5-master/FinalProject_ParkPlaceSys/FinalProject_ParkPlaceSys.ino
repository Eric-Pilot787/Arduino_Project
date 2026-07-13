// =====================================================
// 節點A：HC-SR04 車位占用偵測
// Arduino UNO
// =====================================================

// HC-SR04 腳位
const int TRIG = 9;
const int ECHO = 10;

// 有效距離小於 10 公分時，判定為有車
const float OCCUPIED_DISTANCE_CM = 10.0;

// 每 2 秒量測一次
const unsigned long MEASURE_INTERVAL = 2000;

// 每 10 秒輸出一次目前狀態，作為心跳
const unsigned long HEARTBEAT_INTERVAL = 10000;


// =====================================================
// 車位狀態變數
// =====================================================

// -1：尚未量測
//  0：無車
//  1：有車
int currentOcc = -1;

// 上一次已輸出的狀態
// 初始設為 -1，確保第一次量測一定輸出 JSON
int previousOcc = -1;

// 上一次量測時間
unsigned long lastMeasureTime = 0;

// 上一次輸出心跳時間
unsigned long lastHeartbeatTime = 0;


/**
 * 量測 HC-SR04 距離
 *
 * 回傳值：
 * >= 0：有效距離，單位為公分
 * -1：沒有收到有效回波
 */
float measureDistanceCm() {
  // 先將 TRIG 拉低，確保觸發訊號乾淨
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  // 送出 10 微秒高電位觸發訊號
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  // 最多等待 30000 微秒的回波
  unsigned long duration = pulseIn(ECHO, HIGH, 30000);

  // 沒有收到回波
  if (duration == 0) {
    return -1;
  }

  // 聲速約為 0.0343 公分／微秒
  // 聲波需要來回，因此除以 2
  float distanceCm = duration * 0.0343 / 2.0;

  return distanceCm;
}


/**
 * 輸出單行 JSON 給 Node-RED
 *
 * Node-RED 再將資料發布到 MQTT Topic：
 * parktest787/nodeA/status
 *
 * 有車：
 * {"node":"A","occ":1}
 *
 * 無車：
 * {"node":"A","occ":0}
 */
void sendStatusJson(int occ) {
  Serial.print("{\"node\":\"A\",\"occ\":");
  Serial.print(occ);
  Serial.println("}");
}


/**
 * 執行一次車位偵測
 */
void updateOccupancy() {
  float distanceCm = measureDistanceCm();

  int newOcc;

  // 有效距離且小於 10 公分，判定為有車
  if (distanceCm >= 0 &&
      distanceCm < OCCUPIED_DISTANCE_CM) {
    newOcc = 1;
  }
  else {
    // 距離大於等於 10 公分，
    // 或沒有收到有效回波，判定為無車
    newOcc = 0;
  }

  currentOcc = newOcc;

  // 車位狀態改變時立即輸出 JSON
  if (currentOcc != previousOcc) {
    sendStatusJson(currentOcc);

    previousOcc = currentOcc;

    // 狀態改變時已經輸出一次，
    // 因此重新開始計算 10 秒心跳
    lastHeartbeatTime = millis();
  }
}


void setup() {
  // 必須與 Node-RED Serial In 設定一致
  Serial.begin(115200);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  digitalWrite(TRIG, LOW);

  // 開機後立即進行第一次量測
  updateOccupancy();

  lastMeasureTime = millis();
}


void loop() {
  unsigned long currentTime = millis();

  // 每 2 秒量測一次車位狀態
  if (currentTime - lastMeasureTime >= MEASURE_INTERVAL) {
    lastMeasureTime = currentTime;

    updateOccupancy();
  }

  // 每 10 秒輸出一次目前狀態，作為心跳
  if (currentOcc != -1 &&
      currentTime - lastHeartbeatTime >= HEARTBEAT_INTERVAL) {

    lastHeartbeatTime = currentTime;

    sendStatusJson(currentOcc);
  }
}