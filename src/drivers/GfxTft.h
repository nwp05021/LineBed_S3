#pragma once
#include "drivers/Gfx.h"
#include <TFT_eSPI.h>

// ST7789 + TFT_eSPI + Sprite 기반 Gfx 구현
class GfxTft : public Gfx {
public:
  void begin() override;

  Rect headerRect() const override { return {0,0,170,22}; }
  Rect bodyRect() const override   { return {0,24,170,272}; }
  Rect footerRect() const override { return {0,298,170,22}; }

  void clear(DirtyRegion region) override;
  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) override;
  void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) override;
  void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) override;
  void setTextColor(uint16_t fg, uint16_t bg) override;
  void drawText(int16_t x, int16_t y, const String& s, uint8_t font=2) override;
  void drawCenterString(const String& label, int16_t x, int16_t y, uint8_t font=2) override;
  void flush(DirtyRegion region) override;
  int16_t width() override;
  int16_t height() override;

  void drawQrCode(const String& payload, const Rect& area) override;

private:
  TFT_eSPI _tft = TFT_eSPI();
  TFT_eSprite _spr = TFT_eSprite(&_tft);
  bool _useSprite = true;
};
