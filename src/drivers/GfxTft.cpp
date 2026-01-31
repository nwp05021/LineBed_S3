#include "drivers/GfxTft.h"

void GfxTft::begin() {
  _tft.init();
  _tft.setRotation(0);

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, TFT_BACKLIGHT_ON);

  _spr.setColorDepth(16);
  _spr.createSprite(170, 320);
  _spr.fillSprite(TFT_BLACK);
  _spr.setTextColor(TFT_WHITE, TFT_BLACK);
  _spr.setTextDatum(TL_DATUM);
  _spr.setTextFont(2);
  _spr.pushSprite(0,0);
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
  _spr.fillRect(x, y, w, h, color);
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
  _spr.setTextFont(font);
  _spr.drawString(s, x, y);
}

void GfxTft::drawCenterString(const String& label, int16_t x, int16_t y, uint8_t font) {
  _spr.setTextFont(font);
  _spr.drawCentreString(label, x, y, font);
}

void GfxTft::flush(DirtyRegion region) {
  (void)region;
  _spr.pushSprite(0,0);
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
