#include "BleProvisionService.h"
#include "platform/ble/BleProvision.h"

// Keep WiFiService provisioning progress in sync with BLE provisioning events
#include "services/WifiService.h"

// 네가 이미 가진 이벤트 큐 or 디스패처 사용
// 없으면 임시로 extern 함수 가정
extern void AppEvent_Push(const AppEvent& ev);

static void onBleStatus(BleProvStatus st, const char* msg) {
  (void)msg;

  AppEvent ev{ AppEventType::NONE };

  switch (st) {
    case BleProvStatus::Starting:
      ev.type = AppEventType::BLE_PROV_STARTED; break;
    case BleProvStatus::Advertising:
      ev.type = AppEventType::BLE_PROV_ADVERTISING; break;
    case BleProvStatus::Connected:
      ev.type = AppEventType::BLE_PROV_CONNECTED; break;
    case BleProvStatus::ReceivingCredentials:
      ev.type = AppEventType::BLE_PROV_RECEIVING; break;
    case BleProvStatus::Applying:
      ev.type = AppEventType::BLE_PROV_APPLYING; break;
    case BleProvStatus::WifiConnecting:
      ev.type = AppEventType::BLE_PROV_WIFI_CONNECTING; break;
    case BleProvStatus::GotIP:
      ev.type = AppEventType::BLE_PROV_GOT_IP; break;
    case BleProvStatus::Success:
      ev.type = AppEventType::BLE_PROV_SUCCESS; break;
    case BleProvStatus::Failed:
      ev.type = AppEventType::BLE_PROV_FAILED; break;
    case BleProvStatus::Stopped:
      ev.type = AppEventType::BLE_PROV_STOPPED; break;
    default:
      return;
  }

  AppEvent_Push(ev);

  // Update WifiService status (used by BLE_PROVISION screen)
  if (WifiService::self) {
    switch (st) {
      case BleProvStatus::Starting:
      case BleProvStatus::Advertising:
      case BleProvStatus::Connected:
        WifiService::self->setProvisionState(ProvState::STARTED, true);
        break;
      case BleProvStatus::ReceivingCredentials:
        WifiService::self->setProvisionState(ProvState::CRED_RECV, true);
        break;
      case BleProvStatus::Applying:
        WifiService::self->setProvisionState(ProvState::CRED_SUCCESS, true);
        break;
      case BleProvStatus::GotIP:
      case BleProvStatus::Success:
        WifiService::self->setProvisionState(ProvState::ENDED, false);
        break;
      case BleProvStatus::Failed:
        WifiService::self->setProvisionState(ProvState::CRED_FAIL, false);
        break;
      case BleProvStatus::Stopped:
        WifiService::self->setProvisionState(ProvState::IDLE, false);
        break;
      default:
        break;
    }
  }
}

void BleProvisionService::init() {
  BleProvision::Config cfg;
  cfg.serviceNamePrefix = "SmartFarmLine";
  cfg.pop = "smartfarmv1";   // TODO: 운영 전 변경
  cfg.autoStart = true;
  cfg.stopAfterProvisioned = true;
  cfg.cb = onBleStatus;

  BleProvision::begin(cfg);
}

void BleProvisionService::tick() {
  BleProvision::loop();
}
