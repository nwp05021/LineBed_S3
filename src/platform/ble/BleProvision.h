#pragma once
#include <Arduino.h>
#include <WiFi.h>   // ⭐⭐⭐ 이 한 줄이 모든 문제의 해답 ⭐⭐⭐

enum class BleProvState : uint8_t {
  IDLE,
  STARTING,
  PROV_START,
  CRED_RECV,
  WIFI_CONNECTING,
  WIFI_GOT_IP,
  WIFI_DISCONNECTED,
  PROV_FAIL,
  PROV_END
};

struct BleProvInfo {
  BleProvState state = BleProvState::IDLE;
  String ssid;
  String ip;
  int failReason = 0;
};

class BleProvision {
public:
  void begin();
  void start();
  void resetWifi(bool reboot = true);

  bool hasSavedConfig() const;
  BleProvInfo info() const { return _info; }

private:
  BleProvInfo _info;

  static BleProvision* s_self;
  static void onArduinoEvent(arduino_event_t* e);  // ← 헤더에 Arduino.h가 있으니 OK
};
