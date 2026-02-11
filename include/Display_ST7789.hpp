#pragma once
#include <LovyanGFX.hpp>

class Display_ST7789 : public lgfx::LGFX_Device
{
  lgfx::Panel_ST7789  _panel;
  lgfx::Bus_SPI      _bus;
  lgfx::Light_PWM    _light;

public:
  Display_ST7789(void)
  {
    // ---- SPI BUS ----
    {
      auto cfg = _bus.config();
      cfg.spi_host   = SPI2_HOST;     // ESP32-S3 기본
      cfg.spi_mode   = 0;
      cfg.freq_write = 20000000;      // 안정 우선
      cfg.freq_read  = 16000000;
      cfg.pin_sclk   = 14;
      cfg.pin_mosi   = 13;
      cfg.pin_miso   = -1;
      cfg.pin_dc     = 16;
      _bus.config(cfg);
      _panel.setBus(&_bus);
    }

    // ---- PANEL ----
    {
      auto cfg = _panel.config();
      cfg.pin_cs     = 15;
      cfg.pin_rst    = 8;
      cfg.panel_width  = 170;
      cfg.panel_height = 320;
      cfg.offset_x   = 35;
      cfg.offset_y   = 0;
      cfg.invert     = true;
      cfg.rgb_order  = false;   // BGR
      _panel.config(cfg);
    }

    // ---- BACKLIGHT ----
    {
      auto cfg = _light.config();
      cfg.pin_bl     = 38;
      cfg.freq       = 44100;
      cfg.pwm_channel = 7;
      _light.config(cfg);
      _panel.setLight(&_light);
    }

    setPanel(&_panel);
  }
};
