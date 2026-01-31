#include <TFT_eSPI.h>
#include "ui/UiRenderer.h"

#ifndef UI_TARGET_FPS
#define UI_TARGET_FPS 20
#endif

#define COLOR_FRAME TFT_RED
#define COLOR_BG    TFT_BLACK

constexpr int BTN_W = 70;
constexpr int BTN_H = 24;
constexpr int BTN_GAP = 12;

void UiRenderer::begin(Gfx& gfx) {
  _gfx = &gfx;
}

bool UiRenderer::shouldRender(const UiView& v, uint32_t nowMs) const {
  const uint32_t frameMs = 1000 / UI_TARGET_FPS;
  if ((int32_t)(nowMs - _lastFrameMs) < (int32_t)frameMs) return false;
  return v.dirty != DirtyRegion::NONE;
}

void UiRenderer::render(const UiView& view) {
  if (!_gfx) return;

  const uint32_t now = millis();
  if (!shouldRender(view, now)) return;
  _lastFrameMs = now;

  const DirtyRegion dirty = view.dirty;
  _gfx->clear(dirty);

  drawMainFrame();

  // HEADER
  if (hasDirty(dirty, DirtyRegion::HEADER) || hasDirty(dirty, DirtyRegion::ALL)) {
    _gfx->drawText(4, 4, view.title, 2);
    _gfx->drawText(130, 4, view.rightStatus, 2);
  }

  // BODY
  if (hasDirty(dirty, DirtyRegion::BODY) || hasDirty(dirty, DirtyRegion::ALL)) {
    const Rect br = _gfx->bodyRect();
    const int16_t startX = br.x + 4;
    const int16_t startY = br.y + 6;
    const int16_t lineH = 22; // tuned for font2 on 170x320

    for (uint8_t i = 0; i < view.bodyLineCount && i < UI_BODY_MAX_LINES; ++i) {
      const int16_t y = startY + (int16_t)i * lineH;
      const bool sel = (view.selectedLine == (int8_t)i);
      if (sel) {
        _gfx->fillRect(br.x + 2, y - 2, br.w - 4, lineH, 0xFFFF); // white background
        _gfx->setTextColor(0x0000, 0xFFFF); // black on white
      } else {
        _gfx->setTextColor(0xFFFF, 0x0000); // white on black
      }
      _gfx->drawText(startX, y, view.bodyLines[i], 2);
    }
    // restore default
    _gfx->setTextColor(0xFFFF, 0x0000);
  }

  // FOOTER
  if (hasDirty(dirty, DirtyRegion::FOOTER) || hasDirty(dirty, DirtyRegion::ALL)) {

    const Rect fr = _gfx->footerRect();
    const int y = fr.y + 4;
    const int h = fr.h - 8;
    const int pad = 8;

    // 1) 저장 완료 토스트
    if (view.footerShowToast) {
      _gfx->setTextColor(0xFFFF, 0x0000);
      _gfx->drawText(fr.x + 4, fr.y + 6, view.footerToastText, 2);
    }

    // 2) SAVE / BACK 버튼
    else if (view.footerShowButtons) {
      const int btnW = (fr.w - 12) / 2;

      // SAVE
      {
        const int x = fr.x + 4;
        const bool selected = (view.footerBtnIndex == 0);
        const bool enabled  = view.footerCanSave;

        if (selected && enabled) {
          _gfx->fillRect(x, y, btnW, h, 0xFFFF);
          _gfx->setTextColor(0x0000, 0xFFFF);
        } else {
          _gfx->setTextColor(enabled ? 0xFFFF : 0x7BEF, 0x0000);
        }
        _gfx->drawText(x + pad, y + 6, "SAVE", 2);
      }

      // BACK
      {
        const int x = fr.x + 8 + btnW;
        const bool selected = (view.footerBtnIndex == 1);

        if (selected) {
          _gfx->fillRect(x, y, btnW, h, 0xFFFF);
          _gfx->setTextColor(0x0000, 0xFFFF);
        } else {
          _gfx->setTextColor(0xFFFF, 0x0000);
        }
        _gfx->drawText(x + pad, y + 6, "BACK", 2);
      }

      _gfx->setTextColor(0xFFFF, 0x0000); // restore
    }

    // 3) 기본 힌트
    else {
      _gfx->setTextColor(0xFFFF, 0x0000);
      _gfx->drawText(fr.x + 4, fr.y + 3, view.hint, 2);
    }
  }

  _gfx->flush(dirty);

  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wcast-qual"
  const_cast<UiView&>(view).dirty = DirtyRegion::NONE;
  #pragma GCC diagnostic pop
}

Rect UiRenderer::detailBoxRect() const {
  return {
    10,   // x
    20,   // y
    static_cast<int16_t>(_gfx->width() - 20),
    static_cast<int16_t>(_gfx->height() - 40)
  };
}

void UiRenderer::drawMainFrame() {
  _gfx->fillRect(0, 0, _gfx->width(), _gfx->height(), TFT_BLACK);

  const Rect hr = _gfx->headerRect();
  const Rect br = _gfx->bodyRect();
  const Rect fr = _gfx->footerRect();

  _gfx->drawRect(hr.x, hr.y, hr.w, hr.h, TFT_GREEN);
  _gfx->drawRect(br.x, br.y, br.w, br.h, TFT_RED);
  _gfx->drawRect(fr.x, fr.y, fr.w, fr.h, TFT_BLUE);
}

void UiRenderer::drawDetailFrame() {
  Rect dr = detailBoxRect();

  _gfx->fillRect(dr.x, dr.y, dr.w, dr.h, TFT_DARKGREY);
  _gfx->drawRect(dr.x, dr.y, dr.w, dr.h, TFT_BLUE);
}

void UiRenderer::drawButton(
  int x, int y,
  const char* label,
  bool focused
) {
  if (focused) {
    // 선택됨: 채운 박스
    _gfx->fillRect(x, y, BTN_W, BTN_H, COLOR_FRAME);
  }
  _gfx->drawRect(x, y, BTN_W, BTN_H, COLOR_FRAME);
  _gfx->setTextColor(COLOR_FRAME, COLOR_BG);

  _gfx->drawCenterString(
    label,
    x + BTN_W / 2,
    y + 5,
    LOAD_FONT2
  );
}

// const char* UiRenderer::modeLabel() const {
//   switch (ctx.mode) {
//     case UiMode::LIST: return "LIST";
//     case UiMode::VIEW: return "VIEW";
//     case UiMode::EDIT: return "EDIT";
//   }
//   return "";
// }