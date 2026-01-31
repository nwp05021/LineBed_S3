#include "ui/UiManager.h"
#include "services/SettingsStore.h"
#include "core/AppEvents.h"

static const char* stateTitle(UiState s) {
  switch (s) {
    case UiState::MAIN: return "LineBed";
    case UiState::MENU: return "Menu";
    case UiState::BED_DETAIL: return "Bed Detail";
    case UiState::BED_EDIT: return "Bed Edit";
    case UiState::BLE_PROVISION: return "WiFi Setup";
    case UiState::SET_TEMP: return "Set Temp";
    case UiState::SET_HUMID: return "Set Humid";
    case UiState::SET_LED_ON: return "LED On(sec)";
    case UiState::SET_LED_OFF: return "LED Off(sec)";
    case UiState::WAITING: return "Waiting";
    default: return "UI";
  }
}

void UiManager::begin(SettingsStore& settings) {
  _settings = &settings;

  // Demo dataset (later: bind to real bed discovery/state table)
  for (int i = 0; i < kBedCount; ++i) {
    _beds[i].id = (uint8_t)(i + 1);
    _beds[i].online = (i % 11) != 0; // sprinkle some "offline" beds
    _beds[i].tempC = 22.0f + (float)(i % 7) * 0.7f;
    _beds[i].humid = 55.0f + (float)(i % 9) * 1.2f;
    _beds[i].setTemp10 = _settings->getSetTemp10();
    _beds[i].ledOnSec = _settings->getLedOnSec();
    _beds[i].ledOffSec = _settings->getLedOffSec();
  }

  _bedSelected = 0;
  _bedScroll = 0;
  _detailCursor = 0;
  _editing = false;

  _view.state = UiState::MAIN;
  _view.dirty = DirtyRegion::ALL;
  rebuildView(DirtyRegion::ALL);
}

void UiManager::tick(uint32_t nowMs, const UiContext& ctx) {
  (void)nowMs;
  _ctx = ctx;

  // If BLE provisioning is active and Wi-Fi not connected, force provisioning screen.
  // if (_ctx.provisioningActive && !_ctx.wifiConnected) {
  //   if (_view.state != UiState::PROVISION) setState(UiState::PROVISION);
  // } else {
  //   // leave provisioning screen when Wi-Fi is connected
  //   if (_view.state == UiState::PROVISION && _ctx.wifiConnected) setState(UiState::BED_LIST);
  // }

  // 토스트 자동 종료 (2.5초)
  if (_detail.saveToast && (millis() - _detail.saveToastTs > 2500)) {
    _detail.saveToast = false;
    rebuildView(DirtyRegion::FOOTER);
  }

  // 실펌에선 여기서 _beds[]를 갱신하는 자리
  rebuildView(DirtyRegion::HEADER | DirtyRegion::BODY | DirtyRegion::FOOTER);
}

void UiManager::onInput(const InputEvent& ev) {
  switch (_view.state) {
    case UiState::MAIN: handleBedList(ev); break;
    case UiState::BED_DETAIL: handleBedDetail(ev); break;
    case UiState::BED_EDIT: handleBedEdit(ev); break;
    case UiState::MENU: handleMenu(ev); break;
    case UiState::SET_TEMP: handleEditInt(ev, 10, 40, 1, UiState::MENU); break;
    case UiState::SET_HUMID: handleEditInt(ev, 30, 90, 1, UiState::MENU); break;
    case UiState::SET_LED_ON: handleEditInt(ev, 1, 3600, 1, UiState::MENU); break;
    case UiState::SET_LED_OFF: handleEditInt(ev, 1, 3600, 1, UiState::MENU); break;
    case UiState::BLE_PROVISION:
      if (ev.type == InputEventType::LONG_CLICK) {
        //BleProvService::stop();
        setState(UiState::MAIN);
      }
      break;
    case UiState::WAITING:
    default:
      // ignore
      break;
  }
}

void UiManager::handleAppEvent(const AppEvent& ev) {
  switch (ev.type) {
    case AppEventType::BLE_PROV_STARTED:
    case AppEventType::BLE_PROV_ADVERTISING:
    case AppEventType::BLE_PROV_RECEIVING:
    case AppEventType::BLE_PROV_APPLYING:
    case AppEventType::BLE_PROV_WIFI_CONNECTING:
      setState(UiState::BLE_PROVISION);
      break;

    case AppEventType::BLE_PROV_GOT_IP:
    case AppEventType::BLE_PROV_SUCCESS:
      setState(UiState::DONE);
      break;

    case AppEventType::BLE_PROV_FAILED:
      setState(UiState::ALARM);   // 또는 DONE + error
      break;

    case AppEventType::BLE_PROV_STOPPED:
      setState(UiState::MAIN);
      break;

    default:
      break;
  }
}

void UiManager::setState(UiState s, DirtyRegion dirty) {
  _view.state = s;
  _view.dirty = dirty;

  // Any state change cancels inline editing.
  _editing = false;

  // legacy single-value edit screens
  if (s == UiState::SET_TEMP) _editInt = _settings->getSetTemp10();
  if (s == UiState::SET_HUMID) _editInt = _settings->getSetHumid();
  if (s == UiState::SET_LED_ON) _editInt = _settings->getLedOnSec();
  if (s == UiState::SET_LED_OFF) _editInt = _settings->getLedOffSec();

  if (s == UiState::BED_DETAIL) {
    _detail.focus = DetailFocus::VALUE;
    _detail.valueChanged = false;
    _detail.saveToast = false;
    _footerBtn = 0;
  }

  if (s == UiState::BED_EDIT) {
    _detail.focus = DetailFocus::VALUE; // 편집 시작은 값 영역
    _detail.valueChanged = false;       // 편집 중 rotate하면 true로
    _detail.saveToast = false;
    _footerBtn = 0;
  }

  rebuildView(DirtyRegion::ALL);
}

void UiManager::ensureListWindow() {
  // 170x320 body(240px) with lineH=22 => 10 lines, but keep a bit of breathing room.
  constexpr int visible = 9;
  if (_bedSelected < 0) _bedSelected = 0;
  if (_bedSelected >= kBedCount) _bedSelected = kBedCount - 1;

  if (_bedSelected < _bedScroll) _bedScroll = _bedSelected;
  if (_bedSelected >= _bedScroll + visible) _bedScroll = _bedSelected - visible + 1;
  if (_bedScroll < 0) _bedScroll = 0;
  const int maxScroll = (kBedCount > visible) ? (kBedCount - visible) : 0;
  if (_bedScroll > maxScroll) _bedScroll = maxScroll;
}

void UiManager::rebuildView(DirtyRegion dirty) {
  _view.dirty = _view.dirty | dirty;
  _view.title = stateTitle(_view.state);

  _view.rightStatus = "";
  if (_ctx.wifiConnected) _view.rightStatus += "W";
  if (_ctx.mqttConnected) _view.rightStatus += "M";
  if (_ctx.pendingRemote) _view.rightStatus += "P";

  _view.bodyLineCount = 0;
  _view.selectedLine = -1;

  if (_view.state == UiState::BLE_PROVISION) {
    _view.title = "WiFi Setup";
    _view.bodyLines[0] = _ctx.provisioningActive ? "Provisioning..." : "WiFi Setup";
    String ps = "STATE:";
    switch ((int)_ctx.provState) {
      case 0: ps += "IDLE"; break;
      case 1: ps += "START"; break;
      case 2: ps += "CRED_RECV"; break;
      case 3: ps += "CRED_OK"; break;
      case 4: ps += "CRED_FAIL"; break;
      case 5: ps += "END"; break;
      default: ps += "?"; break;
    }
    _view.bodyLines[1] = ps;
    if (_ctx.wifiConnected) _view.bodyLines[2] = "IP: " + _ctx.wifiIp;
    else if (_ctx.wifiDiscReason) _view.bodyLines[2] = "DISC: " + String(_ctx.wifiDiscReason);
    else _view.bodyLines[2] = "BLE: SmartFarmLine";

    _view.bodyLineCount = 3;
    _view.hint = _ctx.wifiConnected ? "Connected" : "Use WiFi Prov app";
    return;
  }

  if (_view.state == UiState::MAIN) {
    ensureListWindow();
    constexpr int visible = 9;
    const int end = min(kBedCount, _bedScroll + visible);

    for (int i = _bedScroll; i < end; ++i) {
      const int row = i - _bedScroll;
      const BedData& b = _beds[i];
      const char* mark = b.online ? "●" : "○";
      _view.bodyLines[row] = String(mark) + " Bed " + String(b.id) +
                             "  " + String(b.tempC, 1) + "C " +
                             String(b.humid, 0) + "%";
    }
    _view.bodyLineCount = (uint8_t)(end - _bedScroll);
    _view.selectedLine = (int8_t)(_bedSelected - _bedScroll);
    _view.hint = "Click:Detail  Long:Menu";
    return;
  }

  if (_view.state == UiState::BED_DETAIL || _view.state == UiState::BED_EDIT) {
    const BedData& b = _beds[_bedSelected];
    _view.title = String("Bed ") + String(b.id);

    // 3 fields with cursor highlight
    _view.bodyLines[0] = String("Status: ") + (b.online ? "ONLINE" : "OFFLINE") +
                         "  T:" + String(b.tempC, 1) + "C" +
                         "  H:" + String(b.humid, 0) + "%";
    _view.bodyLines[1] = String("Target Temp: ") + String(b.tempC) + "C";
    _view.bodyLines[2] = String("LED Cycle: ") + String(b.ledOnSec) + "/" + String(b.ledOffSec) + "s";
    _view.bodyLineCount = 3;

    // Cursor only on editable fields 1..2
    if (_detailCursor < 1) _detailCursor = 1;
    if (_detailCursor > 2) _detailCursor = 2;
    _view.selectedLine = (int8_t)_detailCursor;

    if (_view.state == UiState::BED_EDIT) {
      _view.hint = "";

      _view.footerShowButtons = true;
      _view.footerBtnIndex = _footerBtn;
      _view.footerCanSave = (_detail.valueChanged && !_detail.saveToast);

      _view.footerShowToast = _detail.saveToast;
      _view.footerToastText = _detail.saveToast ? "저장 완료 ✓" : "";

      if (_detail.focus == DetailFocus::BTN_SAVE) {
        _view.selectedLine = -1; // body highlight 제거
      }
    } else {
      _view.footerShowButtons = false;
      _view.footerShowToast = false;
      _view.footerToastText = "";
      _view.footerCanSave = false;
      _view.footerBtnIndex = 0;

      _view.hint = "Rotate:Move  Click:Edit  Long:Back";
      if (_ctx.pendingRemote) _view.hint += "  ⏳Remote";      
    }

    return;
  }

  if (_view.state == UiState::MENU) {
    static const char* items[] = {
      "Back to List",
      "WiFi Provision",
      "Global Set Temp",
      "Global LED On",
      "Global LED Off",
    };
    const int n = 5;
    if (_menuIndex < 0) _menuIndex = 0;
    if (_menuIndex >= n) _menuIndex = n - 1;

    for (int i = 0; i < n && i < (int)UI_BODY_MAX_LINES; ++i) {
      _view.bodyLines[i] = String((i == _menuIndex) ? "> " : "  ") + items[i];
    }
    _view.bodyLineCount = (uint8_t)n;
    _view.selectedLine = (int8_t)_menuIndex;
    _view.hint = "Rotate:Move  Click:Enter  Long:Back";
    return;
  }

  // Legacy / debug edit screens
  _view.bodyLines[0] = _editing ? "Editing..." : "Click to edit";
  _view.bodyLines[1] = "Value:";
  _view.bodyLines[2] = String(_editInt);
  _view.bodyLineCount = 3;
  _view.selectedLine = 2;
  _view.hint = _editing ? "Rotate:chg  Click:OK  Long:Cancel" : "Click:Edit  Long:Back";
  if (_ctx.pendingRemote) _view.hint += "  ⏳Remote";
}

void UiManager::handleBedList(const InputEvent& ev) {
  if (ev.type == InputEventType::ROTATE) {
    _bedSelected += ev.delta;
    ensureListWindow();
    rebuildView(DirtyRegion::BODY);
    return;
  }
  if (ev.type == InputEventType::CLICK) {
    _detailCursor = 1;
    setState(UiState::BED_DETAIL);
    return;
  }
  if (ev.type == InputEventType::LONG_CLICK) {
    setState(UiState::MENU);
    return;
  }
}

void UiManager::handleBedDetail(const InputEvent& ev) {
  if (ev.type == InputEventType::ROTATE) {
    _detailCursor += ev.delta;
    if (_detailCursor < 1) _detailCursor = 1;
    if (_detailCursor > 2) _detailCursor = 2;
    rebuildView(DirtyRegion::BODY);

    return;
  }
  if (ev.type == InputEventType::LONG_CLICK) {
    setState(UiState::MAIN);
    return;
  }
  if (ev.type == InputEventType::CLICK) {
    // Enter edit mode for the selected field.
    const BedData& b = _beds[_bedSelected];
    if (_detailCursor == 1) _editInt = b.setTemp10;
    else if (_detailCursor == 2) _editInt = 0; // special: led cycle editing uses cursor toggling
    setState(UiState::BED_EDIT);
    return;
  }
}

void UiManager::handleBedEdit(const InputEvent& ev) {
  // LONG = BACK
  if (ev.type == InputEventType::LONG_CLICK) {
    setState(UiState::BED_DETAIL);
    return;
  }

  // ROTATE
  if (ev.type == InputEventType::ROTATE) {
    // 버튼 포커스일 때: SAVE/BACK 토글
    if (_detail.focus == DetailFocus::VALUE) {
      _detail.focus = (ev.delta > 0)
        ? DetailFocus::BTN_SAVE
        : DetailFocus::VALUE;
    }
    else if (_detail.focus == DetailFocus::BTN_SAVE) {
      _detail.focus = (ev.delta > 0)
        ? DetailFocus::BTN_BACK
        : DetailFocus::VALUE;
    }
    else if (_detail.focus == DetailFocus::BTN_BACK) {
      _detail.focus = (ev.delta < 0)
        ? DetailFocus::BTN_SAVE
        : DetailFocus::BTN_BACK;
    }

    // 값 포커스일 때: 기존 편집 로직
    BedData& b = _beds[_bedSelected];

    if (_detailCursor == 1) {
      _editInt += ev.delta;              // 1 step = 0.1℃
      if (_editInt < 100) _editInt = 100;
      if (_editInt > 400) _editInt = 400;
      b.setTemp10 = _editInt;
      _detail.valueChanged = true;
    }
    else if (_detailCursor == 2) {
      int on = (int)b.ledOnSec + ev.delta;
      if (on < 1) on = 1;
      if (on > 3600) on = 3600;
      b.ledOnSec = (uint16_t)on;
      _detail.valueChanged = true;
    }

    rebuildView(DirtyRegion::BODY | DirtyRegion::FOOTER);
    return;
  }

  // CLICK
  if (ev.type == InputEventType::CLICK) {

    // VALUE → BUTTON 포커스 이동
    if (_detail.focus == DetailFocus::VALUE) {
      _detail.focus = DetailFocus::BTN_SAVE;
      _footerBtn = 0; // SAVE 기본 선택
      rebuildView(DirtyRegion::BODY | DirtyRegion::FOOTER);
      return;
    }

    // BUTTON 포커스에서 동작
    if (_detail.focus == DetailFocus::BTN_BACK) {

      // BACK
      if (_footerBtn == 1) {
        setState(UiState::BED_DETAIL);
        return;
      }

      // SAVE
      if (_footerBtn == 0) {
        if (!_detail.valueChanged) return; // 비활성

        // 실제 저장 (SSOT)
        BedData& b = _beds[_bedSelected];
        _settings->setSetTemp10(b.setTemp10);
        _settings->setLedOnSec(b.ledOnSec);
        _settings->setLedOffSec(b.ledOffSec);

        // 토스트 표시
        _detail.valueChanged = false;
        _detail.saveToast = true;
        _detail.saveToastTs = millis();

        rebuildView(DirtyRegion::FOOTER | DirtyRegion::BODY);
        return;
      }
    }
  }
}

void UiManager::handleMenu(const InputEvent& ev) {
  static const int n = 5;
  if (ev.type == InputEventType::ROTATE) {
    _menuIndex += ev.delta;
    if (_menuIndex < 0) _menuIndex = 0;
    if (_menuIndex >= n) _menuIndex = n - 1;
    rebuildView(DirtyRegion::BODY);
    return;
  }
  if (ev.type == InputEventType::LONG_CLICK) {
    setState(UiState::MAIN);
    return;
  }
  if (ev.type == InputEventType::CLICK) {
    switch (_menuIndex) {
      case 0: setState(UiState::MAIN); break;
      case 1:
        // if provisioning is available, UI tick() will drive the actual state based on ctx.
        setState(UiState::BLE_PROVISION);
        break;
      case 2: setState(UiState::SET_TEMP); break;
      case 3: setState(UiState::SET_LED_ON); break;
      case 4: setState(UiState::SET_LED_OFF); break;
      default: break;
    }
    return;
  }
}

void UiManager::handleEditInt(const InputEvent& ev, int minV, int maxV, int step, UiState nextOnExit) {
  if (ev.type == InputEventType::LONG_CLICK) {
    _editOutcome = EditOutcome::CANCEL;
    setState(nextOnExit);
    return;
  }

  if (ev.type == InputEventType::CLICK) {
    if (!_editing) {
      _editing = true;
      rebuildView(DirtyRegion::FOOTER | DirtyRegion::BODY);
      return;
    }

    _editing = false;
    if (_view.state == UiState::SET_TEMP) _settings->setSetTemp10(_editInt);
    if (_view.state == UiState::SET_HUMID) _settings->setSetHumid(_editInt);
    if (_view.state == UiState::SET_LED_ON) _settings->setLedOnSec((uint16_t)_editInt);
    if (_view.state == UiState::SET_LED_OFF) _settings->setLedOffSec((uint16_t)_editInt);
    _editOutcome = EditOutcome::CONFIRM;
    setState(nextOnExit);
    return;
  }

  if (ev.type == InputEventType::ROTATE && _editing) {
    _editInt += ev.delta * step;
    if (_editInt < minV) _editInt = minV;
    if (_editInt > maxV) _editInt = maxV;
    rebuildView(DirtyRegion::BODY);
  }
}

EditOutcome UiManager::consumeEditOutcome() {
  EditOutcome o = _editOutcome;
  _editOutcome = EditOutcome::NONE;
  return o;
}

void UiManager::setInitialState() {
  // Keep UX focused on the bed list.
  setState(UiState::MAIN);
}
