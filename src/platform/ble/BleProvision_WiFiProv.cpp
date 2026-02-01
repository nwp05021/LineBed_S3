#include "BleProvision.h"

#include <WiFi.h>
#include <WiFiProv.h>
#include <esp_wifi.h>
#include <nvs_flash.h>
#include <esp_system.h>

BleProvision* BleProvision::s_self = nullptr;

void BleProvision::onArduinoEvent(arduino_event_t* e) {
  if (!s_self) return;

  switch (e->event_id) {
    case ARDUINO_EVENT_PROV_START:
      s_self->_info.state = BleProvState::PROV_START;
      break;

    case ARDUINO_EVENT_PROV_CRED_RECV:
      s_self->_info.state = BleProvState::CRED_RECV;
      s_self->_info.ssid = (const char*)e->event_info.prov_cred_recv.ssid;
      break;

    case ARDUINO_EVENT_PROV_CRED_FAIL:
      s_self->_info.state = BleProvState::PROV_FAIL;
      s_self->_info.failReason = e->event_info.prov_fail_reason;
      break;

    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      s_self->_info.state = BleProvState::WIFI_GOT_IP;
      s_self->_info.ip = IPAddress(e->event_info.got_ip.ip_info.ip.addr).toString();
      break;

    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      s_self->_info.state = BleProvState::WIFI_DISCONNECTED;
      break;

    case ARDUINO_EVENT_PROV_END:
      s_self->_info.state = BleProvState::PROV_END;
      break;

    default:
      break;
  }
}

void BleProvision::begin() {
  s_self = this;

  WiFi.onEvent(BleProvision::onArduinoEvent);   // ✅ 시그니처 일치
  WiFi.mode(WIFI_STA);
  WiFi.begin();

  _info = BleProvInfo(); // reset
}

void BleProvision::start() {
  _info = BleProvInfo();
  _info.state = BleProvState::STARTING;

  static const char* pop = "abcd1234";
  static const char* serviceName = "PROV_LineBed";
  static const char* serviceKey = nullptr;
  static bool resetProvisioned = false;

  uint8_t uuid[16] = {0xb4,0xdf,0x5a,0x1c,0x3f,0x6b,0xf4,0xbf,0xea,0x4a,0x82,0x03,0x04,0x90,0x1a,0x02};

  WiFiProv.beginProvision(
    WIFI_PROV_SCHEME_BLE,
    WIFI_PROV_SCHEME_HANDLER_FREE_BLE,
    WIFI_PROV_SECURITY_1,
    pop, serviceName, serviceKey,
    uuid, resetProvisioned
  );
}

bool BleProvision::hasSavedConfig() const {
  wifi_config_t cfg{};
  if (esp_wifi_get_config(WIFI_IF_STA, &cfg) != ESP_OK) return false;
  return cfg.sta.ssid[0] != 0;
}

void BleProvision::resetWifi(bool reboot) {
  esp_err_t err = nvs_flash_erase();
  if (err != ESP_OK) nvs_flash_init();

  WiFi.disconnect(true, true);

  if (reboot) {
    delay(200);
    esp_restart();
  }
}
