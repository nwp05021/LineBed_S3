#pragma once
#include <stdint.h>

enum class SystemState : uint8_t {
  BOOTING,
  IDLE,
  PROVISIONING,
  RUNNING,
  ERROR,
};

enum class NetworkState : uint8_t {
    DISCONNECTED,
    CONNECTING,
    CONNECTED,
    ERROR
};

enum class SettingsApplyState : uint8_t {
    IDLE,
    APPLYING,
    APPLIED,
    FAILED
};

struct AppState {
    SystemState system = SystemState::BOOTING;
    NetworkState network = NetworkState::DISCONNECTED;
    SettingsApplyState settingsApply = SettingsApplyState::IDLE;
    int lastError = 0;
};
