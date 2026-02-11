#pragma once
#include "../core/Widget.h"
#include "../../display/FontSpec.h"

class Label : public Widget {
public:
  explicit Label(const char* text) : _text(text) {}
  void setText(const char* t) { _text = t; markDirty(); }

  void draw(IDisplay& d, const UiStore& uiStore) override {
    FontSpec f; f.size = FontSize::SMALL;
    auto r = bounds();
    d.drawText(r.x, r.y, _text, 0xFFFF, 0x0000, f);
  }

private:
  const char* _text;
};
