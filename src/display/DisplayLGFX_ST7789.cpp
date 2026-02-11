//-----------------------------------------------
// DisplayLGFX_ST7789.cpp
//-----------------------------------------------
#include "DisplayLGFX_ST7789.h"
#include "ui/Theme.h"

static constexpr int PIN_MOSI = 13;
static constexpr int PIN_SCLK = 14;
static constexpr int PIN_MISO = -1;
static constexpr int PIN_CS   = 15;
static constexpr int PIN_DC   = 16;
static constexpr int PIN_RST  = 8;
static constexpr int PIN_BL   = 38;

static constexpr int PANEL_W  = 170;
static constexpr int PANEL_H  = 320;
static constexpr int OFFSET_X = 35;
static constexpr int OFFSET_Y = 0;

DisplayLGFX_ST7789::LGFX::LGFX() {
  // BUS
  {
    auto cfg = _bus.config();
    cfg.spi_host   = SPI2_HOST;       // ESP32-S3 기본 SPI host
    cfg.spi_mode   = 0;
    cfg.freq_write = 20000000;        // 안정 우선
    cfg.freq_read  = 16000000;
    cfg.pin_sclk   = PIN_SCLK;
    cfg.pin_mosi   = PIN_MOSI;
    cfg.pin_miso   = PIN_MISO;
    cfg.pin_dc     = PIN_DC;
    _bus.config(cfg);
    _panel.setBus(&_bus);
  }

  // PANEL
  {
    auto cfg = _panel.config();
    cfg.pin_cs      = PIN_CS;
    cfg.pin_rst     = PIN_RST;
    cfg.panel_width  = PANEL_W;
    cfg.panel_height = PANEL_H;
    cfg.offset_x    = OFFSET_X;
    cfg.offset_y    = OFFSET_Y;
    cfg.invert      = true;    // 네가 쓰던 TFT_INVERSION_ON
    cfg.rgb_order   = false;   // false=BGR (네가 TFT_BGR 쓰던 것)
    _panel.config(cfg);
  }

  // BACKLIGHT
  {
    auto cfg = _light.config();
    cfg.pin_bl      = PIN_BL;
    cfg.freq        = 44100;
    cfg.pwm_channel = 7;       // 다른 PWM 쓰면 변경
    _light.config(cfg);
    _panel.setLight(&_light);
  }

  setPanel(&_panel);
}

DisplayLGFX_ST7789::DisplayLGFX_ST7789() = default;
static lgfx::VLWfont s_koreanFont;

//-----------------------------------------------
// begin()
//-----------------------------------------------
bool DisplayLGFX_ST7789::begin() {
  _gfx.init();
  _gfx.setRotation(0);

  _sprite.setColorDepth(16);
  _sprite.createSprite(width(), height());
  _sprite.setTextDatum(top_left);

  _sprite.loadFont("/fonts/NanumGothic-24.vlw");  // 🔑 이것만
  return true;
}

//-----------------------------------------------
// render()
//-----------------------------------------------
void DisplayLGFX_ST7789::render() {
  if (_dirty.rect().empty()) return;

  _dirty.clip(_gfx.width(), _gfx.height());
  if (_dirty.rect().empty()) return;

  const Rect& r = _dirty.rect();

  // 🔒 clear는 여기서만
  _sprite.fillRect(r.x, r.y, r.w, r.h,
                   Theme::colors().background);

  // 🔒 draw는 절대 clear 하지 않음
  //_screen->draw();

  // 🔒 push도 여기서만
  _sprite.pushSprite(r.x, r.y);

  // dirty 초기화
  _dirty.reset();
}

void DisplayLGFX_ST7789::beginFrame() {
  // nothing for now (later: dirty rect)
}

void DisplayLGFX_ST7789::endFrame() {
  //if (!_dirty.valid) return;

  // 한 번에 패널로 전송
  _sprite.pushSprite(0, 0);

  _dirty.reset();
}

void DisplayLGFX_ST7789::setBrightness(uint8_t v) {
  _gfx.setBrightness(v);
}

int DisplayLGFX_ST7789::width() const  { return _gfx.width(); }
int DisplayLGFX_ST7789::height() const { return _gfx.height(); }

void DisplayLGFX_ST7789::clear(uint16_t c) {
  _sprite.fillScreen(c);
  _dirty.merge(0, 0, width(), height());
}

void DisplayLGFX_ST7789::fillRect(int x, int y, int w, int h, uint16_t c) {
  _sprite.fillRect(x, y, w, h, c);
  _dirty.merge(x, y, w, h);
}

void DisplayLGFX_ST7789::drawRect(int x, int y, int w, int h, uint16_t c) {
  _sprite.drawRect(x, y, w, h, c);
  _dirty.merge(x, y, w, h);
}

//-----------------------------------------------
// drawText()
//-----------------------------------------------
void DisplayLGFX_ST7789::drawText(int x, int y, const char* text,
                                  uint16_t fg, uint16_t bg,
                                  const FontSpec& f) {
  switch (f.type) {
    case FontType::KOREAN:
      _sprite.setFont(selectKoreanFont(f.size));
      break;

    case FontType::NUMERIC:
      _sprite.setFont(selectNumericFont(f.size));
      break;

    case FontType::ASCII:
    default:
      _sprite.setFont(nullptr);  // 내장 기본 폰트
      break;
  }

  _sprite.setTextColor(fg, bg);
  _sprite.setCursor(x, y);
  _sprite.print(text);

  int tw = _sprite.textWidth(text);
  int th = _sprite.fontHeight();
  _dirty.merge(x, y, tw, th);
}

const lgfx::v1::IFont* DisplayLGFX_ST7789::selectKoreanFont(FontSize size) {
  switch (size) {
    case FontSize::SMALL:  return &s_koreanFont;
    case FontSize::LARGE:  return &s_koreanFont;
    case FontSize::MEDIUM:
    default:               return &s_koreanFont;
  }
}

const lgfx::v1::IFont* DisplayLGFX_ST7789::selectNumericFont(FontSize size) {
  // V1: 내장 폰트 재사용 (빠름 + 안정)
  return nullptr;
}
