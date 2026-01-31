#pragma once
#include <Arduino.h>
#include "core/AppEvents.h"
#include "ui/UiTypes.h"
#include "ui/InputEvent.h"

class SettingsStore;

enum class EditOutcome : uint8_t { NONE=0, CONFIRM, CANCEL };

struct DetailContext {
    DetailFocus focus = DetailFocus::VALUE;

    bool valueChanged = false;

    bool saveToast = false;
    uint32_t saveToastTs = 0;
};

class UiManager {
public:
  void begin(SettingsStore& settings);
  void tick(uint32_t nowMs, const UiContext& ctx);
  void onInput(const InputEvent& ev);
  void handleAppEvent(const AppEvent& ev);

  const UiView& view() const { return _view; }
  bool isLocalEditing() const { return _editing; }
  EditOutcome consumeEditOutcome();

  void setInitialState();

private:
  SettingsStore* _settings = nullptr;
  UiView _view;
  UiContext _ctx;

  struct BedData {
    uint8_t id = 0;
    bool online = true;
    float tempC = 0;
    float humid = 0;

    // configurable targets (demo values; later bind to real per-bed SSOT)
    int setTemp10 = 240;    // 24.0 C
    uint16_t ledOnSec = 10;
    uint16_t ledOffSec = 20;
  };

  static constexpr int kBedCount = 30;
  BedData _beds[kBedCount];
  int _bedSelected = 0;   // absolute index 0..kBedCount-1
  int _bedScroll = 0;     // top index of list window

  int _detailCursor = 0;  // which field highlighted in BED_DETAIL/BED_EDIT

  int _menuIndex = 0;
  bool _editing = false;
  EditOutcome _editOutcome = EditOutcome::NONE;
  int _editInt = 0;

  void setState(UiState s, DirtyRegion dirty=DirtyRegion::ALL);
  void rebuildView(DirtyRegion dirty);

  void handleBedList(const InputEvent& ev);
  void handleBedDetail(const InputEvent& ev);
  void handleBedEdit(const InputEvent& ev);
  void handleMenu(const InputEvent& ev);
  void handleEditInt(const InputEvent& ev, int minV, int maxV, int step, UiState nextOnExit);

  void ensureListWindow();

  DetailContext _detail;      // v0.1.3c detail focus/changed/toast
  uint8_t _footerBtn = 0;     // 0=SAVE, 1=BACK (버튼 포커스일 때)
};
