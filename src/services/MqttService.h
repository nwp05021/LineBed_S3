#pragma once
#include <Arduino.h>
class SettingsStore;
struct PendingSettings;

class MqttService {
public:
  void begin(SettingsStore& settings);
  void tick(uint32_t nowMs, bool wifiConnected);

  // ⭐ 콜백에서 호출 가능해야 함
  void handleMessage(char* topic, uint8_t* payload, unsigned int len);

  bool isConnected() const;

  bool popIncomingSettings(PendingSettings& out); // remote set/delta
  void publishReportedNow();                      // AWS Shadow reported update

private:
  SettingsStore* _settings = nullptr;
  uint32_t _nextRetryMs = 0;
  bool _connected = false;

  PendingSettings* _incoming = nullptr;
  bool _hasIncoming = false;

  void requestShadowGet();
  void publishShadowReported();
};
