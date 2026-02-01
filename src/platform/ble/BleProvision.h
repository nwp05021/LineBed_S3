#pragma once
#include <stdint.h>

enum class BleProvStatus : uint8_t {
  Idle = 0,
  Init,
  AlreadyProvisioned,
  Starting,
  Advertising,
  Connected,
  ReceivingCredentials,
  Applying,
  WifiConnecting,
  GotIP,
  Success,
  Failed,
  Stopped
};

using BleProvCallback = void(*)(BleProvStatus st, const char* msg);

class BleProvision {
public:
  struct Config {
    const char* serviceNamePrefix = "SmartFarmLine";
    const char* pop = "smartfarmv1"; // Proof of Possession (Security 1)
    bool autoStart = true;
    bool stopAfterProvisioned = true;
    BleProvCallback cb = nullptr;
  };

  // Arduino entry
  static void begin(const Config& cfg);
  static void loop();

  // Optional controls
  static bool isProvisioned();
  static bool hasSavedConfig();
  static void start();
  static void stop();

  // Clear stored Wi-Fi credentials (NVS) so the UI can show "Provision" again.
  static void resetWifi();

private:
  BleProvision() = delete;
};
