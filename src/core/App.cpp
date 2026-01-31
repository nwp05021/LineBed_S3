#include "core/App.h"

#include "drivers/EncoderEc11.h"
#include "drivers/GfxTft.h"
#include "services/BleProvisionService.h"
#include "services/SettingsStore.h"
#include "services/SensorService.h"
#include "services/WifiService.h"
#include "services/MqttService.h"
#include "services/PendingSettings.h"
#include "services/LedCycleService.h"
#include "ui/UiManager.h"
#include "ui/UiRenderer.h"

static SettingsStore gSettings;
static SensorService gSensors;
static WifiService gWifi;
static MqttService gMqtt;
static LedCycleService gLed;

static EncoderEc11 gEncoder;
static GfxTft gGfx;
static UiManager gUi;
static UiRenderer gUiRenderer;

static PendingSettings gPendingRemote;
static bool gHasPendingRemote = false;

void App::begin() {
  gSettings.begin();

  gEncoder.begin();
  gGfx.begin();

  gUi.begin(gSettings);

  // ⭐⭐⭐ 반드시 필요 ⭐⭐⭐
  gUi.setInitialState();
    
  gUiRenderer.begin(gGfx);

  gSensors.begin();
  gLed.begin(gSettings);

  gWifi.begin(gSettings);
  // BLE provisioning should be initialized after WiFiService sets WiFi event handlers
  // so UI can reflect progress via WifiService status.
  BleProvisionService::init();
  gMqtt.begin(gSettings);
}

void App::loop() {
  BleProvisionService::tick();

  const uint32_t now = millis();

  // 1) 입력 수집 (논블로킹)
  InputEvent ev;
  while (gEncoder.poll(ev)) {
    gUi.onInput(ev);
  }

  // 2) 서비스 업데이트(센서/통신/제어)
  gSensors.tick(now);
  gLed.tick(now);

  gWifi.tick(now);
  gMqtt.tick(now, gWifi.isConnected());

  // 3) 원격 설정 수신/적용 정책 (Remote Pending UX)
  PendingSettings incoming;
  if (gMqtt.popIncomingSettings(incoming)) {
    if (gUi.isLocalEditing()) {
      gPendingRemote = incoming;
      gHasPendingRemote = incoming.hasAny;
      Serial.println("[APP] Remote settings pending (local editing)");
    } else {
      if (incoming.hasSetTemp10) gSettings.setSetTemp10(incoming.setTemp10);
      if (incoming.hasSetHumid) gSettings.setSetHumid(incoming.setHumid);
      if (incoming.hasLedOn)    gSettings.setLedOnSec(incoming.ledOnSec);
      if (incoming.hasLedOff)   gSettings.setLedOffSec(incoming.ledOffSec);
      gMqtt.publishReportedNow();
      Serial.println("[APP] Remote settings applied");
    }
  }

  const auto outcome = gUi.consumeEditOutcome();
  if (outcome == EditOutcome::CONFIRM) {
    if (gHasPendingRemote) {
      Serial.println("[APP] Local confirm -> drop pending remote");
      gHasPendingRemote = false;
      gPendingRemote.clear();
    }
    gMqtt.publishReportedNow();
  } else if (outcome == EditOutcome::CANCEL) {
    if (gHasPendingRemote && gPendingRemote.hasAny) {
      Serial.println("[APP] Local cancel -> apply pending remote");
      if (gPendingRemote.hasSetTemp10) gSettings.setSetTemp10(gPendingRemote.setTemp10);
      if (gPendingRemote.hasSetHumid) gSettings.setSetHumid(gPendingRemote.setHumid);
      if (gPendingRemote.hasLedOn)    gSettings.setLedOnSec(gPendingRemote.ledOnSec);
      if (gPendingRemote.hasLedOff)   gSettings.setLedOffSec(gPendingRemote.ledOffSec);
      gHasPendingRemote = false;
      gPendingRemote.clear();
      gMqtt.publishReportedNow();
    }
  }

  // 4) UI 컨텍스트 갱신
  UiContext ctx;
  ctx.tempC = gSensors.lastTempC();
  ctx.humid = gSensors.lastHumid();
  ctx.wifiConnected = gWifi.isConnected();
  ctx.mqttConnected = gMqtt.isConnected();
  ctx.pendingRemote = gHasPendingRemote;
  ctx.provisioningActive = gWifi.isProvisioningActive();
  ctx.hasStoredSsid = gWifi.hasStoredSsid();
  {
    const auto st = gWifi.status();
    ctx.provState = (uint8_t)st.prov;
    ctx.wifiSsid = st.ssid;
    ctx.wifiIp = st.ip;
    ctx.wifiDiscReason = st.lastDisconnectReason;
  }

  ctx.setTemp10 = gSettings.getSetTemp10();
  ctx.setHumid = gSettings.getSetHumid();
  ctx.ledOnSec = gSettings.getLedOnSec();
  ctx.ledOffSec = gSettings.getLedOffSec();

  // 4) UI 상태머신 tick
  gUi.tick(now, ctx);

  // 5) 렌더 (FPS 제한 + DirtyRect)
  gUiRenderer.render(gUi.view());

  // 6) 설정 저장(디바운스)
  gSettings.tick(now);
}
