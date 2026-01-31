#pragma once
#include <stdint.h>

enum class AppEventType : uint8_t {
  NONE = 0,

  // BLE provisioning
  BLE_PROV_STARTED,
  BLE_PROV_ADVERTISING,
  BLE_PROV_CONNECTED,
  BLE_PROV_RECEIVING,
  BLE_PROV_APPLYING,
  BLE_PROV_WIFI_CONNECTING,
  BLE_PROV_GOT_IP,
  BLE_PROV_SUCCESS,
  BLE_PROV_FAILED,
  BLE_PROV_STOPPED
};

struct AppEvent {
  AppEventType type;
};
