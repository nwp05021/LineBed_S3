//-----------------------------------------------
// Screen.h
//
//  - 한 화면 단위(Dashboard, Settings, Provisioning…)
//  - Widgets를 소유하고 레이아웃/포커스 규칙을 결정
//-----------------------------------------------
#pragma once
#include <vector>
#include <memory>
#include "Widget.h"
#include "UiContext.h"
#include "FocusManager.h"

class Screen {
public:
  virtual ~Screen() = default;

  virtual void onEnter(UiContext& ctx) {}
  virtual void onExit(UiContext& ctx) {}

  // Store 변화 통지 (revision 바뀌면 호출)
  virtual void onStoreChanged(const UiContext& ctx) {
    for (auto& w : _widgets) w->onStoreChanged(ctx.store);
    _dirty = true;
  }

  virtual bool handleEvent(UiContext& ctx, const UiEvent& e) {
    int focusIndex = _focus.index();

    // 기본: 포커스 위젯에게 먼저 전달
    if (focusIndex >= 0 && focusIndex < (int)_widgets.size()) {
      if (_widgets[focusIndex]->handleEvent(e, ctx.store)) {
        _dirty = true;
        return true;
      }
    }
    return false;
  }

  virtual void layout(UiContext& ctx, int screenW, int screenH) = 0;

  // 화면 전체 렌더 (dirty 위젯만 그리거나, 화면 단위로 그리거나 선택)
  virtual void draw(UiContext& ctx, IDisplay& d) {
    d.beginFrame();
    drawBackground(ctx, d);
    for (auto& w : _widgets) w->draw(d, ctx.store);
    d.endFrame();

    for (auto& w : _widgets) w->clearDirty();
    _dirty = false;
  }

  bool isDirty() const {
    if (_dirty) return true;
    for (auto& w : _widgets) if (w->isDirty()) return true;
    return false;
  }

  // 포커스 이동 API
  void focusNext() { setFocus(findNextFocusable(+1)); }
  void focusPrev() { setFocus(findNextFocusable(-1)); }
  void setFocus(int idx) {
    int focusIndex = _focus.index();

    if (idx == focusIndex) return;
    if (focusIndex >= 0) _widgets[focusIndex]->setFocused(false);
    _focus.set(idx);
    if (focusIndex >= 0) _widgets[focusIndex]->setFocused(true);
    _dirty = true;
  }

protected:
  virtual void drawBackground(UiContext& ctx, IDisplay& d) {
    d.clear(0x0000);
  }

  int findNextFocusable(int dir) {
    if (_widgets.empty()) return -1;
    int start = _focus.index();
    for (int step=1; step <= (int)_widgets.size(); step++) {
      int i = (start + dir*step + (int)_widgets.size()) % (int)_widgets.size();
      if (_widgets[i]->focusable()) return i;
    }
    return -1;
  }

  std::vector<std::unique_ptr<Widget>> _widgets;
  FocusManager _focus;
  bool _dirty = true;
};
