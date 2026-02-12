#pragma once
#include "../core/UiContext.h"
#include "../core/UiEvent.h"
#include "../../display/IDisplay.h"

class Overlay {
public:
  virtual ~Overlay() = default;

  virtual void onShow(const UiContext&) {}
  virtual void onHide(const UiContext&) {}

  // true 반환하면 이벤트 소비
  virtual bool handleEvent(const UiContext&, const UiEvent&) { return false; }

  virtual void draw(const UiContext&, IDisplay&) = 0;

  virtual bool isExpired(uint32_t nowMs) const { return false; }
};
