#pragma once
#include <Arduino.h>

class SensorService {
public:
  void begin();
  void tick(uint32_t nowMs);

  float lastTempC() const { return _tempC; }
  float lastHumid() const { return _humid; }

private:
  uint32_t _nextMs = 0;
  float _tempC = 23.5f;
  float _humid = 55.0f;
};
