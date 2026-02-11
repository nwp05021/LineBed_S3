#pragma once
#include "SettingsTypes.h"

class SettingsValidator {
public:
    static bool validate(const LineSettings& s) {
        if (s.targetTemperature < 5 || s.targetTemperature > 40) return false;
        if (s.targetHumidity < 20 || s.targetHumidity > 95) return false;
        if (s.ledBrightness > 100) return false;
        if (s.schemaVersion == 0) return false;
        return true;
    }
};
