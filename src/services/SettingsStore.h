#pragma once
#include <Arduino.h>
#include <Preferences.h>

class SettingsStore {
public:
  void begin();
  void tick(uint32_t nowMs);

  int getSetTemp10() const { return _setTemp10; }
  float getSetTempC() const { return _setTemp10 / 10.0f; }

  int getSetHumid() const { return _setHumid; }
  uint16_t getLedOnSec() const { return _ledOnSec; }
  uint16_t getLedOffSec() const { return _ledOffSec; }

  void setSetTemp10(int v10);
  void setSetHumid(int v);
  void setLedOnSec(uint16_t v);
  void setLedOffSec(uint16_t v);

private:
  void load();
  void scheduleSave();

  Preferences _prefs;
  bool _dirty = false;
  uint32_t _saveDueMs = 0;

  int _setTemp10 = 240;
  int _setHumid = 60;
  uint16_t _ledOnSec = 10;
  uint16_t _ledOffSec = 20;
};
