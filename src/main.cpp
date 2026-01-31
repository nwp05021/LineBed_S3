#include <Arduino.h>
#include "platform/ble/BleProvision.h"
#include "core/App.h"

static void onProvStatus(BleProvStatus st, const char* msg) {
  Serial.printf("[BLE_PROV] %d : %s\n", (int)st, msg ? msg : "");
  // TODO: 여기서 UI 상태(FSM)에 이벤트로 전달하면 됨
}

static App app;

void setup() {
  Serial.begin(115200);
  delay(200);

#if SMARTFARM_BLE_PROV
  BleProvision::Config cfg;
  cfg.serviceNamePrefix = "SmartFarmLine";
  cfg.pop = "smartfarmv1";               // 👈 PoP(Proof of Possession). 운영 전 반드시 변경 권장
  cfg.autoStart = true;                 // 부팅 시 자동 시작
  cfg.stopAfterProvisioned = true;      // 성공 시 BLE 종료
  cfg.cb = onProvStatus;

  BleProvision::begin(cfg);
#endif

  Serial.println();
  Serial.println("SmartFarm Line MCU V1 boot");
  Serial.println("[BOOT] setup begin");
  Serial.println("[BOOT] before UI init");
  app.begin();
  Serial.println("[BOOT] after UI init");
  Serial.println("[BOOT] before BLE start");
}

void loop() {
#if SMARTFARM_BLE_PROV
  BleProvision::loop();  // non-blocking (현재는 내부적으로 할 일 거의 없음)
#endif

  app.loop();

  delay(10);
}

