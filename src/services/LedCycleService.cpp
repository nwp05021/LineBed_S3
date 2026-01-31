#include "services/LedCycleService.h"
#include "services/SettingsStore.h"

void LedCycleService::begin(SettingsStore& settings) {
  _settings = &settings;
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
  _on = false;
  _nextToggleMs = millis() + 500;
}

void LedCycleService::tick(uint32_t nowMs) {
  if ((int32_t)(nowMs - _nextToggleMs) < 0) return;

  _on = !_on;
  digitalWrite(PIN_LED, _on ? HIGH : LOW);

  const uint32_t onMs  = (uint32_t)_settings->getLedOnSec() * 1000UL;
  const uint32_t offMs = (uint32_t)_settings->getLedOffSec() * 1000UL;
  _nextToggleMs = nowMs + (_on ? onMs : offMs);
}
