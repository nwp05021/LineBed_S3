#pragma once
#include "platform/ble/BleProvision.h"

class BleProvisionService {
public:
  static void init();
  static void tick();          // ← 추가 (App 호환)

  static void startProvision();
  static void resetWifi();

  static BleProvInfo info();
  static bool hasSavedConfig();
};
