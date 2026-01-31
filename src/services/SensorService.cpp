#include "services/SensorService.h"

void SensorService::begin() {}

void SensorService::tick(uint32_t nowMs) {
  if ((int32_t)(nowMs - _nextMs) < 0) return;
  _nextMs = nowMs + 1000;

  _tempC += 0.05f;
  if (_tempC > 28.0f) _tempC = 22.0f;

  _humid += 0.10f;
  if (_humid > 70.0f) _humid = 45.0f;
}
