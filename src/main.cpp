#include <Arduino.h>
#include <WiFi.h>
#include <WiFiProv.h>

#include "platform/ble/BleProvision.h"
#include "services/BleProvisionService.h"
#include "core/App.h"

static App app;

void setup() {
  Serial.begin(115200);
  delay(200);

  Serial.println();
  Serial.println("SmartFarm Line MCU V1 boot");
  Serial.println("[BOOT] setup begin");
  Serial.println("[BOOT] before UI init");
  app.begin();
  Serial.println("[BOOT] after UI init");
  Serial.println("[BOOT] before BLE start");

}

void loop() {
  auto info = BleProvisionService::info();

  switch (info.state) {
    case BleProvState::PROV_START:
      break;
    case BleProvState::WIFI_GOT_IP:
      break;
    default:
      break;
  }

  app.loop();

  delay(10);
}
