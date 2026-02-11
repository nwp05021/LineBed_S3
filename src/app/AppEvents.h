#pragma once
#include <stdint.h>
#include "services/settings/SettingsTypes.h"

enum class AppEventType : uint8_t {
    NONE,
    SETTINGS_APPLY_REQUEST,
};

struct AppEvent {
    AppEventType type = AppEventType::NONE;
    LineSettings settings;
};

void AppEvent_Push(const AppEvent& ev);
bool AppEvent_Pop(AppEvent& ev);
