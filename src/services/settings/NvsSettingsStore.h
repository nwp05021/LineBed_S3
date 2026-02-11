#pragma once
#include "SettingsTypes.h"

class NvsSettingsStore {
public:
    bool begin();
    bool load(LineSettings& out);
    bool save(const LineSettings& in);
};
