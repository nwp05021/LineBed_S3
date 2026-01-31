#include "services/SettingsStore.h"

#define SETTINGS_SCHEMA_VERSION 2

static constexpr const char* NS = "sf_line";
static constexpr uint32_t SAVE_DEBOUNCE_MS = 1500;

void SettingsStore::begin() {
  _prefs.begin(NS, false);
  load();
}

void SettingsStore::load() {
  const uint16_t schema = _prefs.getUShort("schema", 0);

  if (schema == 1) {
    // v1 → v2 migration
    const int oldTemp = _prefs.getInt("setTemp", 24);
    _setTemp10 = oldTemp * 10;

    _setHumid  = _prefs.getInt("setHumid", _setHumid);
    _ledOnSec  = _prefs.getUShort("ledOn", _ledOnSec);
    _ledOffSec = _prefs.getUShort("ledOff", _ledOffSec);

    _prefs.putUShort("schema", 2);
    _prefs.putInt("setTemp10", _setTemp10);
    _prefs.putInt("setHumid", _setHumid);
    _prefs.putUShort("ledOn", _ledOnSec);
    _prefs.putUShort("ledOff", _ledOffSec);
    return;
  }

  if (schema != 2) {
    // unknown → reset to defaults in v2 format
    _prefs.putUShort("schema", 2);
    _prefs.putInt("setTemp10", _setTemp10);
    _prefs.putInt("setHumid", _setHumid);
    _prefs.putUShort("ledOn", _ledOnSec);
    _prefs.putUShort("ledOff", _ledOffSec);
    return;
  }

  _setTemp10 = _prefs.getInt("setTemp10", _setTemp10);
  _setHumid  = _prefs.getInt("setHumid", _setHumid);
  _ledOnSec  = _prefs.getUShort("ledOn", _ledOnSec);
  _ledOffSec = _prefs.getUShort("ledOff", _ledOffSec);
}

void SettingsStore::scheduleSave() {
  _dirty = true;
  _saveDueMs = millis() + SAVE_DEBOUNCE_MS;
}

void SettingsStore::tick(uint32_t nowMs) {
  if (_dirty && (int32_t)(nowMs - _saveDueMs) >= 0) {
    _dirty = false;
    _prefs.putInt("setTemp10", _setTemp10);
    _prefs.putInt("setHumid", _setHumid);
    _prefs.putUShort("ledOn", _ledOnSec);
    _prefs.putUShort("ledOff", _ledOffSec);
  }
}

void SettingsStore::setSetTemp10(int v10) {
  _setTemp10 = v10;
  scheduleSave();
}

void SettingsStore::setSetHumid(int v) { _setHumid = v; scheduleSave(); }
void SettingsStore::setLedOnSec(uint16_t v) { _ledOnSec = v; scheduleSave(); }
void SettingsStore::setLedOffSec(uint16_t v) { _ledOffSec = v; scheduleSave(); }
