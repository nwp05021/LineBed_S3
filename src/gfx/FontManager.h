//-----------------------------------------------
// gfx/FontManager.h
// 👉 한글 안정성의 핵심
//-----------------------------------------------
#pragma once
#include <Arduino.h>
#include <FS.h>
#include "FontId.h"

class FontManager {
public:
    static bool begin(FS& fs);

    static void select(FontId id);
    static int16_t measureText(const char* utf8);
    static void drawText(int16_t x, int16_t y,
                         const char* utf8,
                         uint16_t color);

private:
    static FS* _fs;
};
