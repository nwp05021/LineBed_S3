#include "services/WifiService.h"
#include "services/SettingsStore.h"

#include <WiFi.h>

#if defined(ENABLE_BLE_PROV)
  //#include <WiFiProv.h>
  static constexpr const char* PROV_SERVICE_NAME = "SmartFarmLine";
  static constexpr const char* PROV_POP = "abcd1234";
#endif

WifiService* WifiService::self = nullptr;

void WifiService::onWiFiEvent(arduino_event_id_t event) {
  if (!self) return;
  
  auto& st = self->status();

  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      st.isConnected = false;
      st.lastDisconnectReason = 0; // 또는 WIFI_REASON_UNSPECIFIED
      Serial.println("[WiFi] Disconnected");
      break;

    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      Serial.println("[WiFi] Connected");
      break;

    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      st.isConnected = true;
      Serial.println("[WiFi] Got IP");
      break;

    default:
      break;
  }

#ifdef ARDUINO_EVENT_PROV_START
  if (event == ARDUINO_EVENT_PROV_START) { st.prov = ProvState::STARTED; sSelf->_provActive = true; }
#endif
#ifdef ARDUINO_EVENT_PROV_CRED_RECV
  if (event == ARDUINO_EVENT_PROV_CRED_RECV) { st.prov = ProvState::CRED_RECV; }
#endif
#ifdef ARDUINO_EVENT_PROV_CRED_SUCCESS
  if (event == ARDUINO_EVENT_PROV_CRED_SUCCESS) { st.prov = ProvState::CRED_SUCCESS; }
#endif
#ifdef ARDUINO_EVENT_PROV_CRED_FAIL
  if (event == ARDUINO_EVENT_PROV_CRED_FAIL) { st.prov = ProvState::CRED_FAIL; }
#endif
#ifdef ARDUINO_EVENT_PROV_END
  if (event == ARDUINO_EVENT_PROV_END) { st.prov = ProvState::ENDED; sSelf->_provActive = false; }
#endif
}

void WifiService::begin(SettingsStore& settings) {
  _settings = &settings;
  self = this;

  _st = {};
  _st.ssid = WiFi.SSID();

  WiFi.onEvent(WifiService::onWiFiEvent);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

#if defined(ENABLE_BLE_PROV)
  if (!WiFi.SSID().length()) {
    Serial.println("[WiFi] No stored SSID. Starting BLE provisioning...");
    _provActive = true;
    _st.prov = ProvState::STARTED;
    // WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM,
    //                         WIFI_PROV_SECURITY_1, PROV_POP,
    //                         PROV_SERVICE_NAME, nullptr);
  } else {
    _provActive = false;
    Serial.printf("[WiFi] Stored SSID found: %s\n", WiFi.SSID().c_str());
    WiFi.begin();
  }
#else
  if (WiFi.SSID().length()) {
    _provActive = false;
    Serial.printf("[WiFi] Stored SSID found: %s\n", WiFi.SSID().c_str());
    WiFi.begin();
  } else {
    Serial.println("[WiFi] No stored SSID. ENABLE_BLE_PROV=1 to enable provisioning.");
  }
#endif
}

void WifiService::tick(uint32_t nowMs) { ensureConnected(nowMs); }

WifiStatus& WifiService::status() {
  return _st;
}

bool WifiService::isConnected() const { return WiFi.status() == WL_CONNECTED; }
bool WifiService::hasStoredSsid() const { return WiFi.SSID().length() > 0; }

void WifiService::ensureConnected(uint32_t nowMs) {
  if (isConnected()) return;
  if ((int32_t)(nowMs - _nextRetryMs) < 0) return;
  _nextRetryMs = nowMs + 5000;

#if !defined(ENABLE_BLE_PROV)
  if (WiFi.SSID().length()) {
    Serial.println("[WiFi] retry connect...");
    WiFi.disconnect(false, false);
    WiFi.begin();
  }
#endif
}

void WifiService::clearCredentials()
{
  WiFi.disconnect(true, true); // erase Wi-Fi NVS
}

