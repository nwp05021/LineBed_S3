#include "SettingsService.h"
#include "SettingsValidator.h"

bool SettingsService::begin() {
    store.begin();
    LineSettings loaded{};
    if (store.load(loaded) && SettingsValidator::validate(loaded)) {
        runtime = loaded;
    }
    return true;
}

SettingsResult SettingsService::apply(const LineSettings& desired, bool persist) {
    if (!SettingsValidator::validate(desired)) {
        return SettingsResult::INVALID_VALUE;
    }

    LineSettings backup = runtime;
    runtime = desired;

    if (persist) {
        if (!store.save(runtime)) {
            runtime = backup;
            return SettingsResult::STORE_ERROR;
        }
    }
    return SettingsResult::OK;
}
