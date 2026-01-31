#pragma once
#include "drivers/Gfx.h"
#include "ui/UiTypes.h"

class UiRenderer {
public:
  void begin(Gfx& gfx);
  void render(const UiView& view);

private:
  Gfx* _gfx = nullptr;
  uint32_t _lastFrameMs = 0;

  bool shouldRender(const UiView& v, uint32_t nowMs) const;
  Rect detailBoxRect() const;
  void drawMainFrame();
  void drawDetailFrame();
  void drawButton(
    int x, int y,
    const char* label,
    bool focused
  );  
  const char* modeLabel() const;
};
