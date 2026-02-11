#pragma once
#include <stdint.h>
#include "FontSpec.h"

class IDisplay {
public:
  virtual ~IDisplay() = default;

  virtual bool begin() = 0;
  virtual void render() = 0;
  virtual void setBrightness(uint8_t v) = 0;

  virtual int  width()  const = 0;
  virtual int  height() const = 0;

  // ----- Frame control -----
  virtual void beginFrame() = 0;
  virtual void endFrame() = 0;

  // ----- Drawing -----
  virtual void clear(uint16_t color565 = 0x0000) = 0;
  virtual void fillRect(int x, int y, int w, int h, uint16_t color565) = 0;
  virtual void drawRect(int x, int y, int w, int h, uint16_t color565) = 0;
  virtual void drawText(int x, int y, const char* text,
                        uint16_t color565, uint16_t bg565,
                        const FontSpec& font) = 0;
};

