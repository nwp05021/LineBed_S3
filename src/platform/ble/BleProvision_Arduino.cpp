#include "BleProvision.h"

// Arduino-first implementation (stable on PlatformIO + arduino-esp32)
// - Uses WiFiProv.h which wraps ESP-IDF provisioning under Arduino.
// - Keeps all provisioning specifics isolated in platform/ble/.

#if defined(ARDUINO) && defined(ESP32)

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiProv.h>

const char * pop = "abcd1234";              // Proof of possession - otherwise called a PIN - string provided by the device, entered by user in the phone app
const char * service_name = "PROV_EggIncb"; // Name of your device (the Espressif apps expects by default device name starting with "Prov_")
const char * service_key = NULL;            // Password used for SofAP method (NULL = no password needed)
bool reset_provisioned = false;             // When true the library will automatically delete previously provisioned data.

static BleProvision::Config s_cfg;
static bool s_started = false;
static bool s_provisioned = false;
static bool s_boundEvents = false;

static void notify(BleProvStatus st, const char* msg) {
  if (s_cfg.cb) s_cfg.cb(st, msg);
}

static void SysProvEvent(arduino_event_t* sys_event) {
  switch (sys_event->event_id) {
    case ARDUINO_EVENT_WIFI_STA_START:
      notify(BleProvStatus::WifiConnecting, "WiFi STA start");
      break;

    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      notify(BleProvStatus::WifiConnecting, "WiFi connected");
      break;

    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      notify(BleProvStatus::GotIP, "Got IP");
      s_provisioned = true;
      // Provisioning will naturally end after success.
      break;

    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      notify(BleProvStatus::WifiConnecting, "WiFi disconnected");
      break;

    case ARDUINO_EVENT_PROV_START:
      notify(BleProvStatus::Starting, "Provisioning started");
      notify(BleProvStatus::Advertising, "BLE advertising");
      break;

    case ARDUINO_EVENT_PROV_CRED_RECV:
      // Do not print SSID/PW (sensitive)
      notify(BleProvStatus::ReceivingCredentials, "Credentials received");
      break;

    case ARDUINO_EVENT_PROV_CRED_SUCCESS:
      notify(BleProvStatus::Applying, "Credentials accepted");
      break;

    case ARDUINO_EVENT_PROV_CRED_FAIL:
      notify(BleProvStatus::Failed, "Provisioning failed");
      break;

    case ARDUINO_EVENT_PROV_END:
      notify(BleProvStatus::Success, "Provisioning ended");
      break;

    default:
      break;
  }
}

void BleProvision::begin(const Config& cfg) {
  s_cfg = cfg;

  if (!s_boundEvents) {
    WiFi.onEvent(SysProvEvent);
    s_boundEvents = true;
  }

  notify(BleProvStatus::Init, "Init");

  // Try normal connect first (will use stored creds if provisioned)
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin();

  if (cfg.autoStart) {
    start();
  }
}

void BleProvision::loop() {
  // No polling required; Arduino events drive state.
}

bool BleProvision::isProvisioned() {
  // Under Arduino, if stored creds exist, WiFi.begin() will eventually connect.
  // We treat "already provisioned" as "has ever gotten IP since boot" OR "has SSID".
  if (s_provisioned) return true;
  return WiFi.SSID().length() > 0;
}

void BleProvision::start()
{
    if (s_started) return;
    s_started = true;

    WiFi.onEvent(SysProvEvent);
    WiFi.begin();

    uint8_t uuid[16] = {
        0xb4, 0xdf, 0x5a, 0x1c,
        0x3f, 0x6b, 0xf4, 0xbf,
        0xea, 0x4a, 0x82, 0x03,
        0x04, 0x90, 0x1a, 0x02
    };

    WiFiProv.beginProvision(
        WIFI_PROV_SCHEME_SOFTAP,
        WIFI_PROV_SCHEME_HANDLER_NONE,
        WIFI_PROV_SECURITY_1,
        pop,
        service_name,
        service_key,
        uuid,
        reset_provisioned
    );

    WiFiProv.printQR(service_name, pop, "ble");
}

void BleProvision::stop() {
  // WiFiProv currently doesn't expose a stable stop API across all arduino-esp32 versions.
  // We only emit a state transition for UI and let the stack end naturally.
  notify(BleProvStatus::Stopped, "Stopped");
}

#endif // ARDUINO && ESP32
