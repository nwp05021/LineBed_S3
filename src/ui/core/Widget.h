//-----------------------------------------------
// Widget.h
//
//  - UI 구성요소(라벨, 버튼, 값 표시줄 등)
//  - draw(), handleEvent(), setDirty()로 관리
//-----------------------------------------------
#pragma once
#include <stdint.h>
#include "UiEvent.h"
#include "UiStore.h"
#include "../../display/IDisplay.h"
#include "../../display/DirtyRect.h"

class Widget {
public:
  virtual ~Widget() = default;

  void setBounds(Rect r) { _bounds = r; markDirty(); }
  Rect bounds() const { return _bounds; }

  bool isDirty() const { return _dirty; }
  void clearDirty() { _dirty = false; }

  virtual bool focusable() const { return false; }
  virtual void setFocused(bool v) { _focused = v; markDirty(); }
  bool focused() const { return _focused; }

  virtual void onStoreChanged(const UiStore&) {}   // 필요 시
  virtual bool handleEvent(const UiEvent&, UiStore&) { return false; }

  virtual void draw(IDisplay& d, const UiStore& store) = 0;

protected:
  void markDirty() { _dirty = true; }

  Rect _bounds{0,0,0,0};
  bool _dirty = true;
  bool _focused = false;
};
