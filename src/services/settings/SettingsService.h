#pragma once
#include "SettingsTypes.h"
#include "NvsSettingsStore.h"

class SettingsService {
public:
    bool begin();
    const LineSettings& get() const { return runtime; }
    SettingsResult apply(const LineSettings& desired, bool persist);

private:
    LineSettings runtime{};
    NvsSettingsStore store{};
};
