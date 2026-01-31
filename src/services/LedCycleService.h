#pragma once
#include <Arduino.h>
class SettingsStore;

class LedCycleService {
public:
  void begin(SettingsStore& settings);
  void tick(uint32_t nowMs);

private:
  SettingsStore* _settings = nullptr;
  static constexpr int PIN_LED = 2;

  bool _on = false;
  uint32_t _nextToggleMs = 0;
};
