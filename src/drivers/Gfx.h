#pragma once
#include <Arduino.h>

enum class DirtyRegion : uint8_t { NONE=0, HEADER=1, BODY=2, FOOTER=4, ALL=7 };
inline DirtyRegion operator|(DirtyRegion a, DirtyRegion b) { return (DirtyRegion)((uint8_t)a | (uint8_t)b); }
inline bool hasDirty(DirtyRegion d, DirtyRegion f) { return (((uint8_t)d) & ((uint8_t)f)) != 0; }

struct Rect { int16_t x,y,w,h; };

class Gfx {
public:
  virtual ~Gfx() = default;
  virtual void begin() = 0;

  virtual Rect headerRect() const = 0;
  virtual Rect bodyRect() const = 0;
  virtual Rect footerRect() const = 0;

  virtual void clear(DirtyRegion region) = 0;
  // Basic primitives (needed for list highlight / UI widgets)
  virtual void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) = 0;
  virtual void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) = 0;
  virtual void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) = 0;
  virtual void setTextColor(uint16_t fg, uint16_t bg) = 0;
  virtual void drawText(int16_t x, int16_t y, const String& s, uint8_t font=2) = 0;
  virtual void drawCenterString(const String& label, int16_t x, int16_t y, uint8_t font=2) = 0;
  virtual void flush(DirtyRegion region) = 0;
  virtual int16_t width() = 0;
  virtual int16_t height() = 0;

  // QR Code (default: no-op)
  virtual void drawQrCode(const String& payload, const Rect& area) { (void)payload; (void)area; }
};
