//-----------------------------------------------
// gfx/FontManager.cpp
// 👉 한글 안정성의 핵심
//-----------------------------------------------
#include "FontManager.h"

FS* FontManager::_fs = nullptr;

bool FontManager::begin(FS& fs) {
    _fs = &fs;
    return true;
}

void FontManager::select(FontId id) {
    switch (id) {
        case FontId::Title:
            //tft.loadFont("/fonts/NotoSansKR_Title.vlw", *_fs);
            break;
        case FontId::Body:
            //tft.loadFont("/fonts/NotoSansKR_Body.vlw", *_fs);
            break;
        case FontId::Small:
            //tft.loadFont("/fonts/NotoSansKR_Small.vlw", *_fs);
            break;
    }
}

int16_t FontManager::measureText(const char* utf8) {
    return 0;
}

void FontManager::drawText(int16_t x, int16_t y,
                           const char* utf8,
                           uint16_t color) {
}
