//
#include <Arduino.h>
#include "GfxDriver.h"

GfxDriver::GfxDriver(IDisplay& display) : _d(display) {}

bool GfxDriver::begin() {
  if (!_d.begin()) return false;
  _d.setBrightness(200);
  _d.clear(0x0000); // black
  return true;
}

//-----------------------------------------------
// tick()
//-----------------------------------------------
void GfxDriver::tick() {
  // 예: 주기적으로 깜빡이게 테스트
  static uint32_t last = 0;
  if (millis() - last > 1000) {
    last = millis();
    _dirty = true;
  }
}

//-----------------------------------------------
// render()
//-----------------------------------------------
void GfxDriver::render() {
  if (!_dirty) return;

  _d.beginFrame();

  _d.clear(0x0000);

  FontSpec f;
  f.size = FontSize::SMALL;

  _d.fillRect(0, 0, _d.width(), 24, 0xFFFF);
  _d.drawText(4, 6, "LINEBED UI (buffered)", 0x0000, 0xFFFF, f);

  f.size = FontSize::MEDIUM;
  _d.drawRect(10, 40, _d.width()-20, 80, 0x07E0);
  _d.drawText(16, 60, "HELLO(가나)", 0xFFFF, 0x0000, f);

  _d.endFrame();

  _dirty = false;
}
