#include "drivers/GfxTft.h"
#include <Arduino.h>   // ESP.getFreeHeap()

#include "drivers/GfxTft.h"
#include <Arduino.h>   // ESP.getFreeHeap()

void GfxTft::begin() {
#ifdef USE_DMA
  Serial.println("[GFX] USE_DMA defined");
#else
  Serial.println("[GFX] USE_DMA NOT defined");
#endif
  
  _tft.init();
  _tft.setRotation(0);

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, TFT_BACKLIGHT_ON);

  Serial.printf("[GFX] heap(before sprite)=%u\n", (unsigned)ESP.getFreeHeap());

  _useSprite = true;

  // 1) 16bit 시도
  _spr.setColorDepth(16);
  if (!_spr.createSprite(170, 320)) {
    Serial.printf("[GFX] Sprite(16bit) alloc failed. heap=%u\n", (unsigned)ESP.getFreeHeap());

    // 2) 8bit로 다운그레이드
    _spr.setColorDepth(8);
    if (!_spr.createSprite(170, 320)) {
      Serial.printf("[GFX] Sprite(8bit) alloc failed. fallback to direct TFT. heap=%u\n",
                    (unsigned)ESP.getFreeHeap());
      _useSprite = false;
    }
  }

  if (_useSprite) {
    _spr.fillSprite(TFT_BLACK);
    _spr.setTextColor(TFT_WHITE, TFT_BLACK);
    _spr.setTextDatum(TL_DATUM);
    _spr.setTextFont(2);
    _spr.pushSprite(0, 0);
  } else {
    // 스프라이트 없이도 최소한 화면은 살아있게
    _tft.fillScreen(TFT_BLACK);
    _tft.setTextColor(TFT_WHITE, TFT_BLACK);
  }

  Serial.printf("[GFX] heap(after init)=%u, useSprite=%d\n",
                (unsigned)ESP.getFreeHeap(), _useSprite ? 1 : 0);
}

void GfxTft::clear(DirtyRegion region) {
  if (region == DirtyRegion::NONE) return;
  if (region == DirtyRegion::ALL) { _spr.fillSprite(TFT_BLACK); return; }

  if (hasDirty(region, DirtyRegion::HEADER)) {
    auto r = headerRect(); _spr.fillRect(r.x,r.y,r.w,r.h, TFT_BLACK);
  }
  if (hasDirty(region, DirtyRegion::BODY)) {
    auto r = bodyRect(); _spr.fillRect(r.x,r.y,r.w,r.h, TFT_BLACK);
  }
  if (hasDirty(region, DirtyRegion::FOOTER)) {
    auto r = footerRect(); _spr.fillRect(r.x,r.y,r.w,r.h, TFT_BLACK);
  }
}

void GfxTft::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  if (_useSprite) _spr.fillRect(x, y, w, h, color);
  else _tft.fillRect(x, y, w, h, color);
}

void GfxTft::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  _spr.drawRect(x, y, w, h, color);
}

void GfxTft::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
  _spr.drawLine(x0, y0, x1, y1, color);
}

void GfxTft::setTextColor(uint16_t fg, uint16_t bg) {
  _spr.setTextColor(fg, bg);
}

void GfxTft::drawText(int16_t x, int16_t y, const String& s, uint8_t font) {
  if (_useSprite) {
    _spr.setTextFont(font);
    _spr.drawString(s, x, y);
  } else {
    _tft.setTextFont(font);
    _tft.drawString(s, x, y);
  }
}

void GfxTft::drawCenterString(const String& label, int16_t x, int16_t y, uint8_t font) {
  _spr.setTextFont(font);
  _spr.drawCentreString(label, x, y, font);
}

void GfxTft::flush(DirtyRegion region) {
  (void)region;
  if (_useSprite) _spr.pushSprite(0, 0);
  // direct TFT는 flush 불필요
}

int16_t GfxTft::width() {
  return _spr.width();
}

int16_t GfxTft::height() {
  return _spr.height();
}

void GfxTft::drawQrCode(const String& payload, const Rect& area)
{
}
