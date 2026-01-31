#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <esp_event.h>        // ⭐ 이 줄이 핵심

class SettingsStore;

enum class WifiLinkState : uint8_t { DISCONNECTED=0, CONNECTING, GOT_IP };
enum class ProvState : uint8_t { IDLE=0, STARTED, CRED_RECV, CRED_SUCCESS, CRED_FAIL, ENDED };

struct WifiStatus {
  bool isConnected = false;     // ⭐ 실제 필드명
  WifiLinkState link = WifiLinkState::DISCONNECTED;
  ProvState prov = ProvState::IDLE;
  String ssid;
  String ip;
  int lastDisconnectReason = 0;
};

class WifiService {
public:
  void begin(SettingsStore& settings);
  void tick(uint32_t nowMs);

  bool isConnected() const;
  bool isProvisioningActive() const { return _provActive; }
  bool hasStoredSsid() const;

  WifiStatus& status();   // ⭐ 선언도 reference
  static void onWiFiEvent(arduino_event_id_t event);

  // ⭐ 전역 접근용
  static WifiService* self;

private:
  SettingsStore* _settings = nullptr;
  uint32_t _nextRetryMs = 0;
  bool _provActive = false;
  WifiStatus _st;

  void ensureConnected(uint32_t nowMs);
  void clearCredentials();
};
