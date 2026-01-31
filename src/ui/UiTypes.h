#pragma once
#include <Arduino.h>
#include "drivers/Gfx.h"

enum class UiState : uint8_t {
  MAIN=0,
  MENU,
  BED_DETAIL,
  BED_EDIT,
  // legacy single-setting screens (kept for reference / quick debug)
  SET_TEMP,
  SET_HUMID,
  SET_LED_ON,
  SET_LED_OFF,

  BLE_PROVISION,
  WAITING,
  DONE,
  ALARM
};

enum class UiMode {
  LIST,
  VIEW,
  EDIT
};

enum class DetailFocus {
  VALUE,
  BTN_SAVE,
  BTN_BACK
};

static constexpr uint8_t UI_BODY_MAX_LINES = 12;

struct UiContext {
  float tempC = 0;
  float humid = 0;
  bool wifiConnected = false;
  bool mqttConnected = false;
  bool pendingRemote = false;
  bool provisioningActive = false;
  bool hasStoredSsid = false;
  uint8_t provState = 0;
  String wifiSsid;
  String wifiIp;
  int wifiDiscReason = 0;

  int setTemp10 = 240;
  int setHumid = 60;
  uint16_t ledOnSec = 10;
  uint16_t ledOffSec = 20;

  DetailFocus detailFocus = DetailFocus::VALUE;

  String provQrPayload;
};

struct UiView {
  UiState state = UiState::MAIN;
  DirtyRegion dirty = DirtyRegion::ALL;

  String title;
  String rightStatus;

  // BODY lines
  String bodyLines[UI_BODY_MAX_LINES];
  uint8_t bodyLineCount = 0;
  // if >=0, highlight that line with inverted style
  int8_t selectedLine = -1;

  String hint;

  // Footer UI extension (v0.1.3c)
  bool footerShowButtons = false; // BED_EDIT에서 true
  bool footerCanSave = false;     // valueChanged && !toast
  uint8_t footerBtnIndex = 0;     // 0=SAVE, 1=BACK
  
  bool footerShowToast = false;
  String footerToastText = "";

  bool bodyShowQr = false;
  String bodyQrPayload = "";
};
