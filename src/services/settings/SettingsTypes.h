#pragma once
#include <stdint.h>

struct LineSettings {
    int16_t targetTemperature = 23;
    int16_t targetHumidity = 60;
    uint8_t ledBrightness = 50;
    uint32_t schemaVersion = 1;
};

enum class SettingsResult : uint8_t {
    OK,
    INVALID_VALUE,
    STORE_ERROR
};
