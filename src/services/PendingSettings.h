#pragma once
#include <Arduino.h>

struct PendingSettings {
  bool hasAny = false;
  bool hasSetTemp10 = false;
  bool hasSetHumid = false;
  bool hasLedOn = false;
  bool hasLedOff = false;

  int setTemp10 = 0;   // 0.1°C 단위
  int setHumid = 0;
  uint16_t ledOnSec = 0;
  uint16_t ledOffSec = 0;

  void clear() { *this = PendingSettings(); }
};
