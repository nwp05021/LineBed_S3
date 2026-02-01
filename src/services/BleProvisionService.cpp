#include "BleProvisionService.h"

static BleProvision s_ble;

void BleProvisionService::init() {
  s_ble.begin();
}

void BleProvisionService::tick() {
  // WiFi.onEvent 기반이므로 polling 필요 없음
  // App 호환을 위해 빈 함수로 유지
}

void BleProvisionService::startProvision() {
  s_ble.start();
}

void BleProvisionService::resetWifi() {
  s_ble.resetWifi(true);
}

BleProvInfo BleProvisionService::info() {
  return s_ble.info();
}

bool BleProvisionService::hasSavedConfig() {
  return s_ble.hasSavedConfig();
}
