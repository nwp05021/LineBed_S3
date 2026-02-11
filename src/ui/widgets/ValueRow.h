#pragma once
#include "../core/Widget.h"
#include "../../display/FontSpec.h"
#include <stdio.h>

class ValueRow : public Widget {
public:
  ValueRow(const char* title, float* valuePtr) : _title(title), _value(valuePtr) {}

  void draw(IDisplay& d, const UiStore& uiStore) override {
    auto r = bounds();
    // 배경
    uint16_t bg = focused() ? 0x39E7 : 0x0000;  // 회색톤(대충)
    d.fillRect(r.x, r.y, r.w, r.h, bg);

    FontSpec f; f.size = FontSize::SMALL;
    d.drawText(r.x+4, r.y+4, _title, 0xFFFF, bg, f);

    char buf[32];
    snprintf(buf, sizeof(buf), "%.1f", _value ? *_value : 0.0f);
    d.drawText(r.x + r.w - 60, r.y+4, buf, 0xFFFF, bg, f);
  }

  bool focusable() const override { return true; }

private:
  const char* _title;
  float* _value;
};
