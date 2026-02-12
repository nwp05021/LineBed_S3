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
#include "display/DirtyRect.h"

class Screen {
public:
  virtual ~Screen() = default;

  virtual void onEnter(UiContext& ctx) {}
  virtual void onExit(UiContext& ctx) {}

  virtual void onStoreChanged(const UiContext& ctx) {
    for (auto& w : _widgets) {
      w->onStoreChanged(ctx.store);
      if (w->isDirty()) {
        const auto& r = w->bounds();
        _dirtyRect.merge(r.x, r.y, r.w, r.h);
      }
    }
  }

  virtual bool handleEvent(const UiContext& ctx, const UiEvent& e) {
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

  virtual void layout(const UiContext& ctx, int screenW, int screenH) = 0;

  virtual void draw(const UiContext& ctx, IDisplay& d) {

    // 아무 변화도 없으면 그리지 않음
    if (!_dirty && _dirtyRect.rect().empty())
      return;

    d.beginFrame();

    // 1️⃣ 전체 리드로우 필요
    if (_dirty) {
      drawBackground(ctx, d);

      for (auto& w : _widgets)
        w->draw(d, ctx.store);
    }
    else {
      // 2️⃣ 부분 리드로우
      _dirtyRect.clip((int16_t)d.width(), (int16_t)d.height());
      Rect dr = _dirtyRect.rect();

      if (!dr.empty()) {

        // 배경 해당 영역만 복구 (단색 배경 가정)
        d.fillRect(dr.x, dr.y, dr.w, dr.h, 0x0000);

        for (auto& w : _widgets) {
          if (w->isDirty())
            w->draw(d, ctx.store);
        }
      }
    }

    d.endFrame();

    for (auto& w : _widgets)
      w->clearDirty();

    _dirtyRect.reset();
    _dirty = false;
  }

  bool isDirty() const {
    if (_dirty) return true;
    if (!_dirtyRect.rect().empty()) return true;

    for (auto& w : _widgets)
      if (w->isDirty()) return true;

    return false;
  }

  // 포커스 이동 API
  void focusNext() { setFocus(findNextFocusable(+1)); }
  void focusPrev() { setFocus(findNextFocusable(-1)); }

  void setFocus(int idx) {
    int old = _focus.index();
    if (idx == old) return;

    // 이전 포커스
    if (old >= 0 && old < (int)_widgets.size()) {
      _widgets[old]->setFocused(false);
      _widgets[old]->markDirty();

      const auto& r = _widgets[old]->bounds();
      _dirtyRect.merge(r.x, r.y, r.w, r.h);
    }

    _focus.set(idx);

    // 새 포커스
    if (idx >= 0 && idx < (int)_widgets.size()) {
      _widgets[idx]->setFocused(true);
      _widgets[idx]->markDirty();

      const auto& r = _widgets[idx]->bounds();
      _dirtyRect.merge(r.x, r.y, r.w, r.h);
    }
  }

  //---------------------------------------------
  // Dirty/Refresh 처리 안정화
  //---------------------------------------------
  virtual void onOverlayShown() {}
  virtual void onOverlayHidden() {}

protected:
  virtual void drawBackground(const UiContext& ctx, IDisplay& d) {
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
  DirtyRect _dirtyRect;
};
