#include "NvsSettingsStore.h"
#include <Preferences.h>

static Preferences prefs;

bool NvsSettingsStore::begin() {
    return true;
}

bool NvsSettingsStore::load(LineSettings& out) {
    if (!prefs.begin("linebed", true)) return false;
    size_t len = prefs.getBytesLength("settings");
    if (len != sizeof(LineSettings)) {
        prefs.end();
        return false;
    }
    prefs.getBytes("settings", &out, sizeof(LineSettings));
    prefs.end();
    return true;
}

bool NvsSettingsStore::save(const LineSettings& in) {
    if (!prefs.begin("linebed", false)) return false;
    size_t written = prefs.putBytes("settings", &in, sizeof(LineSettings));
    prefs.end();
    return written == sizeof(LineSettings);
}
