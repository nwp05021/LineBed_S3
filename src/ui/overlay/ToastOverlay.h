#pragma once
#include "Overlay.h"

class ToastOverlay : public Overlay {
public:
  ToastOverlay(const char* msg, uint16_t durationMs)
    : _msg(msg), _duration(durationMs) {}

  void onShow(const UiContext&) override {
    _start = millis();
  }

  void draw(const UiContext&, IDisplay& d) override {

    int w = d.width();
    int h = d.height();

    // 배경 바
    d.fillRect(8, h - 30, w - 16, 22, 0x0000);

    // 테두리
    d.drawRect(8, h - 30, w - 16, 22, 0xFFFF);

    // 텍스트
    FontSpec f;
    f.size = FontSize::SMALL;

    d.drawText(16, h - 25, _msg, 0xFFFF, 0x0000, f);
  }

  bool isExpired(uint32_t nowMs) const override {
    return (nowMs - _start) >= _duration;
  }

private:
  const char* _msg;
  uint16_t _duration;
  uint32_t _start = 0;
};
