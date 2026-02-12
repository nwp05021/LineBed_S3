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
  const Rect& bounds() const { return _bounds; }

  bool isDirty() const { return _dirty; }
  void clearDirty() { _dirty = false; }

  virtual bool focusable() const { return false; }
  virtual void setFocused(bool v) { _focused = v; markDirty(); }
  bool focused() const { return _focused; }

  virtual void onStoreChanged(const UiStore&) {}
  virtual bool handleEvent(const UiEvent&, const UiStore&) { return false; }

  virtual void draw(IDisplay& d, const UiStore& store) = 0;

  // 🔥 반드시 public
  void markDirty() { _dirty = true; }

protected:
  Rect _bounds{0,0,0,0};
  bool _dirty = true;
  bool _focused = false;
};
