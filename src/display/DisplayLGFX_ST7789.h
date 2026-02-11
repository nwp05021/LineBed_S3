#pragma once

#include "FontSpec.h"
#include "IDisplay.h"
#include <LovyanGFX.hpp>
#include "DirtyRect.h"

class DisplayLGFX_ST7789 final : public IDisplay {
public:
  DisplayLGFX_ST7789();

  bool begin() override;
  void render() override;
  void setBrightness(uint8_t v) override;

  int width()  const override;
  int height() const override;

  // ----- frame control -----
  void beginFrame() override;
  void endFrame() override;

  // ----- drawing -----
  void clear(uint16_t color565) override;
  void fillRect(int x, int y, int w, int h, uint16_t color565) override;
  void drawRect(int x, int y, int w, int h, uint16_t color565) override;
  void drawText(int x, int y, const char* text,
                uint16_t color565, uint16_t bg565,
                const FontSpec& font) override;

private:
  class LGFX : public lgfx::LGFX_Device {
    lgfx::Panel_ST7789  _panel;
    lgfx::Bus_SPI       _bus;
    lgfx::Light_PWM     _light;
  public:
    LGFX();
  };

  LGFX _gfx;
  lgfx::LGFX_Sprite _sprite{&_gfx};   // 🔥 생성자에서 부모 전달
  DirtyRect _dirty;

  const lgfx::v1::IFont* selectKoreanFont(FontSize size);
  const lgfx::v1::IFont* selectNumericFont(FontSize size);
};
