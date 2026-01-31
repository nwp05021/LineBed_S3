#include "BleProvision.h"
#include <cstring>

#if defined(ARDUINO) && defined(ESP32)

extern "C" {
  #include "esp_log.h"
  #include "esp_err.h"
  #include "esp_event.h"
  #include "esp_netif.h"
  #include "nvs_flash.h"
  #include "esp_wifi.h"
  #include "esp_mac.h"

  #include "wifi_provisioning/manager.h"
  #include "wifi_provisioning/scheme_ble.h"
}

static const char* TAG = "BleProvision";

// ---------- module state ----------
static BleProvision::Config s_cfg;
static bool s_inited = false;
static bool s_started = false;
static bool s_provisioned = false;
static bool s_stopAfter = true;

// ---------- helpers ----------
static void notify(BleProvStatus st, const char* msg) {
  if (s_cfg.cb) s_cfg.cb(st, msg);
}

static void make_service_name(char* out, size_t outLen) {
  uint8_t mac[6] = {0};
  esp_read_mac(mac, ESP_MAC_WIFI_STA);
  // e.g. SmartFarmLine-1A2B3C
  snprintf(out, outLen, "%s-%02X%02X%02X", s_cfg.serviceNamePrefix,
           mac[3], mac[4], mac[5]);
}

// ---------- event handler ----------
static void sys_event_handler(void* arg,
                              esp_event_base_t event_base,
                              int32_t event_id,
                              void* event_data)
{
  (void)arg;
  (void)event_data;

  if (event_base == WIFI_PROV_EVENT) {
    switch (event_id) {
      case WIFI_PROV_START:
        notify(BleProvStatus::Starting, "Provisioning started");
        notify(BleProvStatus::Advertising, "BLE advertising");
        break;

      case WIFI_PROV_CRED_RECV:
        // Do NOT log SSID/PASS (sensitive)
        notify(BleProvStatus::ReceivingCredentials, "Credentials received");
        break;

      case WIFI_PROV_CRED_SUCCESS:
        notify(BleProvStatus::Applying, "Credentials applied");
        break;

      case WIFI_PROV_CRED_FAIL:
        notify(BleProvStatus::Failed, "Credential validation failed");
        break;

      case WIFI_PROV_END:
        notify(BleProvStatus::Success, "Provisioning ended");
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
        // non-blocking
        esp_wifi_connect();
        break;

      case WIFI_EVENT_STA_DISCONNECTED:
        notify(BleProvStatus::WifiConnecting, "WiFi disconnected -> reconnect");
        // non-blocking retry (policy can be refined later)
        esp_wifi_connect();
        break;

      default:
        break;
    }
    return;
  }

  if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    notify(BleProvStatus::GotIP, "Got IP");
    s_provisioned = true;

    if (s_stopAfter) {
      // stop BLE provisioning to save resources
      BleProvision::stop();
    }
    return;
  }
}

static esp_err_t ensure_idf_basics() {
  notify(BleProvStatus::Init, "Init: NVS/Netif/EventLoop");

  // NVS (Arduino core might already init it)
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  if (err != ESP_OK) return err;

  // Netif/Event loop (may already exist)
  err = esp_netif_init();
  if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) return err;

  err = esp_event_loop_create_default();
  if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) return err;

  return ESP_OK;
}

static esp_err_t prov_mgr_init_if_needed() {
  // BLE scheme (Arduino+IDF mixed env safe form)
  wifi_prov_mgr_config_t config = {};
  config.scheme = wifi_prov_scheme_ble;
  config.scheme_event_handler = WIFI_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BTDM;

  esp_err_t err = wifi_prov_mgr_init(config);
  if (err == ESP_ERR_INVALID_STATE) {
    // Already initialized — ok
    return ESP_OK;
  }
  return err;
}

static bool query_is_provisioned() {
  bool provisioned = false;

  esp_err_t err = prov_mgr_init_if_needed();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "wifi_prov_mgr_init failed: %s", esp_err_to_name(err));
    return false;
  }

  err = wifi_prov_mgr_is_provisioned(&provisioned);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "is_provisioned failed: %s", esp_err_to_name(err));
    return false;
  }
  return provisioned;
}

// ---------- public API ----------
void BleProvision::begin(const Config& cfg) {
  s_cfg = cfg;
  s_stopAfter = cfg.stopAfterProvisioned;

  if (s_inited) return;

  esp_err_t err = ensure_idf_basics();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "ensure_idf_basics: %s", esp_err_to_name(err));
    notify(BleProvStatus::Failed, "Init failed");
    return;
  }

  // Register handlers once
  ESP_ERROR_CHECK(esp_event_handler_register(WIFI_PROV_EVENT, ESP_EVENT_ANY_ID, &sys_event_handler, nullptr));
  ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &sys_event_handler, nullptr));
  ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &sys_event_handler, nullptr));

  s_inited = true;

  // Check if already provisioned
  s_provisioned = query_is_provisioned();
  if (s_provisioned) {
    notify(BleProvStatus::AlreadyProvisioned, "Already provisioned");
    if (cfg.autoStart) {
      // Trigger wifi connect path (non-blocking)
      notify(BleProvStatus::WifiConnecting, "Connecting WiFi");
      esp_wifi_connect();
    }
    return;
  }

  if (cfg.autoStart) start();
  else notify(BleProvStatus::Idle, "Ready (autoStart=false)");
}

void BleProvision::start() {
  if (!s_inited) {
    notify(BleProvStatus::Failed, "Not initialized. Call begin() first.");
    return;
  }
  if (s_started) return;

  esp_err_t err = prov_mgr_init_if_needed();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "wifi_prov_mgr_init: %s", esp_err_to_name(err));
    notify(BleProvStatus::Failed, "wifi_prov_mgr_init failed");
    return;
  }

  char service_name[32];
  make_service_name(service_name, sizeof(service_name));

  // Security 1 (PoP)
  // NOTE: In mixed Arduino env, pass PoP as const void*.
  //       Do NOT rely on wifi_prov_security1_params_t availability.
  wifi_prov_security_t security = WIFI_PROV_SECURITY_1;
  const void* security_params = (const void*)s_cfg.pop;

  err = wifi_prov_mgr_start_provisioning(
    security, 
    s_cfg.pop,          // 👈 const char* 그대로 전달
    service_name, 
    nullptr
  );
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "start_provisioning: %s", esp_err_to_name(err));
    notify(BleProvStatus::Failed, "start_provisioning failed");
    return;
  }

  s_started = true;
  notify(BleProvStatus::Starting, "Provisioning starting");
}

void BleProvision::stop() {
  if (!s_inited) return;

  // Stop provisioning (ignore errors)
  wifi_prov_mgr_stop_provisioning();
  wifi_prov_mgr_deinit();

  s_started = false;
  notify(BleProvStatus::Stopped, "Provisioning stopped");
}

bool BleProvision::isProvisioned() {
  return s_provisioned;
}

void BleProvision::loop() {
  // Event-driven. Keep non-blocking.
}

#else

// Non-ESP32 builds (or non-Arduino)
void BleProvision::begin(const Config&) {}
void BleProvision::loop() {}
bool BleProvision::isProvisioned() { return false; }
void BleProvision::start() {}
void BleProvision::stop() {}

#endif
