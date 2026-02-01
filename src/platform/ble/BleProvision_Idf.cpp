#include "BleProvision.h"
#include <cstring>   // for memset

// ESP-IDF based BLE Provisioning implementation (Arduino as primary framework)
// - Keeps all IDF calls isolated under src/platform/ble/
// - Non-blocking: progress is driven by ESP-IDF/Arduino event callbacks
// - Provides stable primitives for UI:
//    * hasSavedConfig() : whether Wi-Fi credentials exist in NVS
//    * resetWifi()      : wipe credentials so the UI can show "Provision" again

#if defined(ARDUINO) && defined(ESP32) && defined(ENABLE_BLE_PROV)

// extern "C" {
//   #include "esp_err.h"
//   #include "esp_event.h"
//   #include "esp_log.h"
//   #include "esp_netif.h"
//   #include "esp_system.h"
//   #include "esp_wifi.h"
//   #include "esp_idf_version.h"
//   #include "nvs_flash.h"

//   #include "wifi_provisioning/manager.h"
//   #include "wifi_provisioning/scheme_ble.h"
// }

//   #include "esp_netif.h"
//   #include "esp_idf_version.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"

#include "wifi_provisioning/manager.h"
#include "wifi_provisioning/scheme_ble.h"

static const char* TAG = "BleProvision";

static BleProvision::Config s_cfg;
static bool s_inited = false;
static bool s_started = false;
static bool s_isProvisioned = false;

static void notify(BleProvStatus st, const char* msg) {
  if (s_cfg.cb) s_cfg.cb(st, msg);
}

static void make_service_name(char* out, size_t outLen) {
  // service name: <prefix>_<last3mac>
  uint8_t mac[6] = {0};
  esp_read_mac(mac, ESP_MAC_WIFI_STA);
  snprintf(out, outLen, "%s_%02X%02X%02X", s_cfg.serviceNamePrefix ? s_cfg.serviceNamePrefix : "SmartFarmLine",
           mac[3], mac[4], mac[5]);
}

static void ensure_idf_ready()
{
  static bool inited = false;
  if (inited) return;

  ESP_ERROR_CHECK(nvs_flash_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  inited = true;
}

static void deinit_mgr_if_needed() {
  // Safe even if not started
  if (!s_inited) return;
  wifi_prov_mgr_deinit();
  s_inited = false;
  s_started = false;
}

static void prov_event_handler(void* /*arg*/, esp_event_base_t event_base, int32_t event_id, void* event_data) {
  if (event_base == WIFI_PROV_EVENT) {
    switch (event_id) {
      case WIFI_PROV_START:
        notify(BleProvStatus::Starting, "Provisioning started");
        notify(BleProvStatus::Advertising, "BLE advertising");
        break;

      case WIFI_PROV_CRED_RECV:
        // Do not log SSID/PW (sensitive)
        notify(BleProvStatus::ReceivingCredentials, "Credentials received");
        break;

      case WIFI_PROV_CRED_SUCCESS:
        notify(BleProvStatus::Applying, "Credentials accepted");
        break;

      case WIFI_PROV_CRED_FAIL:
        notify(BleProvStatus::Failed, "Provisioning failed");
        break;

      case WIFI_PROV_END:
        // Provisioning flow ended (success or user stopped)
        notify(BleProvStatus::Success, "Provisioning ended");
        if (s_cfg.stopAfterProvisioned) {
          wifi_prov_mgr_deinit();
          s_inited = false;
          s_started = false;
        }
        break;

      default:
        break;
    }
    return;
  }

  if (event_base == WIFI_EVENT) {
    switch (event_id) {
      case WIFI_EVENT_STA_START:
        notify(BleProvStatus::WifiConnecting, "WiFi STA start");
        break;
      case WIFI_EVENT_STA_CONNECTED:
        notify(BleProvStatus::WifiConnecting, "WiFi connected");
        break;
      case WIFI_EVENT_STA_DISCONNECTED:
        notify(BleProvStatus::WifiConnecting, "WiFi disconnected");
        break;
      default:
        break;
    }
    return;
  }

  if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    s_isProvisioned = true;
    notify(BleProvStatus::GotIP, "Got IP");
    return;
  }
}

void BleProvision::begin(const Config& cfg) {
  s_cfg = cfg;
  notify(BleProvStatus::Init, "Init");

  ensure_idf_ready();

  // Register once (duplicate registration is harmless, but keep simple)
  static bool s_evtBound = false;
  if (!s_evtBound) {
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_PROV_EVENT, ESP_EVENT_ANY_ID, &prov_event_handler, nullptr));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &prov_event_handler, nullptr));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &prov_event_handler, nullptr));
    s_evtBound = true;
  }

  // If already provisioned, we don't need to start provisioning.
  bool prov = false;
  esp_err_t err = wifi_prov_mgr_is_provisioned(&prov);
  if (err == ESP_OK && prov) {
    notify(BleProvStatus::AlreadyProvisioned, "Already provisioned");
    s_isProvisioned = true;
    if (s_cfg.stopAfterProvisioned) {
      // Manager isn't required if already provisioned.
      deinit_mgr_if_needed();
    }
    return;
  }

  if (cfg.autoStart) start();
}

void BleProvision::loop() {
  // Non-blocking: all transitions come from event callbacks.
}

bool BleProvision::hasSavedConfig() {
  wifi_config_t cfg;
  memset(&cfg, 0, sizeof(cfg));

  // esp_wifi_get_config requires Wi-Fi driver init.
  // Arduino usually inits Wi-Fi lazily; if not ready, return false.
  esp_err_t err = esp_wifi_get_config(WIFI_IF_STA, &cfg);
  if (err != ESP_OK) return false;
  return cfg.sta.ssid[0] != 0;
}

bool BleProvision::isProvisioned() {
  if (s_isProvisioned) return true;
  bool prov = false;
  if (s_inited && wifi_prov_mgr_is_provisioned(&prov) == ESP_OK) {
    return prov;
  }
  // Fallback: if Wi-Fi config exists.
  return hasSavedConfig();
}

void BleProvision::start() {
  if (s_started) return;
  ensure_idf_ready();

  // If provisioned, do not start BLE advertising.
  bool prov = false;
  if (wifi_prov_mgr_is_provisioned(&prov) == ESP_OK && prov) {
    notify(BleProvStatus::AlreadyProvisioned, "Already provisioned");
    s_isProvisioned = true;
    return;
  }

  char service_name[32];
  make_service_name(service_name, sizeof(service_name));

  // Security 1 uses Proof-of-Possession (POP)
  wifi_prov_security_t security = WIFI_PROV_SECURITY_1;
  const char* service_key = nullptr; // not used for BLE

  esp_err_t err = wifi_prov_mgr_start_provisioning(security, s_cfg.pop, service_name, service_key);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "start_provisioning failed: %s", esp_err_to_name(err));
    notify(BleProvStatus::Failed, "start_provisioning failed");
    return;
  }

  s_started = true;
  notify(BleProvStatus::Starting, "Provisioning started");
}

void BleProvision::stop() {
  // Stop provisioning session and free BLE stack memory.
  // NOTE: wifi_prov_mgr_stop_provisioning() is available in ESP-IDF; if missing in older IDF,
  // deinit() is still safe to end the session.
  if (!s_inited) {
    notify(BleProvStatus::Stopped, "Stopped");
    return;
  }

  #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 0)
    (void)wifi_prov_mgr_stop_provisioning();
  #endif

  deinit_mgr_if_needed();
  notify(BleProvStatus::Stopped, "Stopped");
}

void BleProvision::resetWifi() {
  // Wipe stored credentials from NVS.
  // After this, UI should show "Provision" again.
  notify(BleProvStatus::Init, "Reset WiFi");

  // Best-effort: disconnect/stop, then restore default Wi-Fi config.
  (void)esp_wifi_disconnect();
  (void)esp_wifi_stop();
  (void)esp_wifi_restore();

  // Also end any ongoing provisioning session.
  if (s_inited) {
    deinit_mgr_if_needed();
  }

  s_isProvisioned = false;
}

#else

// Provisioning disabled: provide no-op stubs to keep build stable.

static BleProvision::Config s_cfg;
static void notify(BleProvStatus st, const char* msg) {
  if (s_cfg.cb) s_cfg.cb(st, msg);
}

void BleProvision::begin(const Config& cfg) { s_cfg = cfg; notify(BleProvStatus::Init, "Init"); }
void BleProvision::loop() {}
bool BleProvision::isProvisioned() { return false; }
bool BleProvision::hasSavedConfig() { return false; }
void BleProvision::start() { notify(BleProvStatus::Failed, "BLE prov disabled"); }
void BleProvision::stop() { notify(BleProvStatus::Stopped, "Stopped"); }
void BleProvision::resetWifi() {}

#endif
