#include "services/WifiService.h"
#include "services/SettingsStore.h"

#include <WiFi.h>

#if defined(ENABLE_BLE_PROV)
  //#include <WiFiProv.h>
  static constexpr const char* PROV_SERVICE_NAME = "SmartFarmLine";
  static constexpr const char* PROV_POP = "abcd1234";
#endif

WifiService* WifiService::self = nullptr;

void WifiService::onWiFiEvent(arduino_event_t* event) {
  if (!self || !event) return;
  auto& st = self->status();

  switch (event->event_id) {
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      st.isConnected = false;
      // If available, capture disconnect reason
      st.lastDisconnectReason = (int)event->event_info.wifi_sta_disconnected.reason;
      st.link = WifiLinkState::DISCONNECTED;
      Serial.println("[WiFi] Disconnected");
      break;

    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      st.link = WifiLinkState::CONNECTING;
      Serial.println("[WiFi] Connected");
      break;

    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      st.isConnected = true;
      st.link = WifiLinkState::GOT_IP;
      st.ssid = WiFi.SSID();
      st.ip = WiFi.localIP().toString();
      Serial.println("[WiFi] Got IP");
      break;

    default:
      break;
  }
}

void WifiService::setProvisionState(ProvState p, bool active) {
  _st.prov = p;
  _provActive = active;
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

  // Wi-Fi connection attempt.
  // If credentials exist, Arduino core will connect.
  // If not, BleProvisionService will drive provisioning, and then WiFi will connect automatically.
  if (WiFi.SSID().length()) {
    _provActive = false;
    Serial.printf("[WiFi] Stored SSID found: %s\n", WiFi.SSID().c_str());
    WiFi.begin();
  } else {
    Serial.println("[WiFi] No stored SSID. Waiting for provisioning...");
    _provActive = false;
    _st.prov = ProvState::IDLE;
  }
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

  if (WiFi.SSID().length()) {
    Serial.println("[WiFi] retry connect...");
    WiFi.disconnect(false, false);
    WiFi.begin();
  }
}

void WifiService::clearCredentials()
{
  WiFi.disconnect(true, true); // erase Wi-Fi NVS
}

