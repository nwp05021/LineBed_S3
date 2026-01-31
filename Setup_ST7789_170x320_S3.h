#pragma once

// ===== Driver =====
#define ST7789_DRIVER

// ===== Panel size (logical) =====
#define TFT_WIDTH   170
#define TFT_HEIGHT  320

// ===== Offset (핵심!) =====
// ST7789 GRAM 240x320 기준에서 좌우 중앙 정렬
#define TFT_OFFSET_X 35
#define TFT_OFFSET_Y 0

// ===== SPI pins (예시, 네 보드에 맞게 수정) =====
#define TFT_MOSI 11
#define TFT_SCLK 12
#define TFT_CS   10
#define TFT_DC    9
#define TFT_RST   8
#define TFT_BL   38

// ===== SPI speed (안정 우선) =====
#define SPI_FREQUENCY  40000000

#define TFT_BACKLIGHT_ON HIGH   
#define TFT_RGB_ORDER TFT_BGR   // 일부 패널은 필수
#define TFT_INVERSION_ON        // ★ 매우 중요 (안 보이면 거의 이것)

// ===== Fonts =====
#define LOAD_GLCD    // Font 1
#define LOAD_FONT2   // small 16 px
#define LOAD_FONT4   // medium 26 px
#define LOAD_FONT6   // large 48 px
#define LOAD_GFXFF   // FreeFonts (한글 폰트 쓸 거면 필수)
 