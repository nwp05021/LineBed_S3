#pragma once
#include "Overlay.h"
#include "../core/UiCommand.h"

class DialogOverlay : public Overlay {
public:
  DialogOverlay(const char* title,
                const char* message,
                bool confirmOnly)
    : _title(title),
      _message(message),
      _confirmOnly(confirmOnly)
  {
    _focusIndex = 0;
    if (confirmOnly) {
      _maxIndex = 0;
    } else {
      _maxIndex = 1;
    }
  }

  bool handleEvent(const UiContext& ctx, const UiEvent& e) override {

    // === 포커스 이동 ===
    if (e.type == UiEventType::EncoderCW) {
      if (_maxIndex > 0) {
        _focusIndex = (_focusIndex + 1) % (_maxIndex + 1);
      }
      return true;
    }

    if (e.type == UiEventType::EncoderCCW) {
      if (_maxIndex > 0) {
        _focusIndex = (_focusIndex == 0 ? _maxIndex : _focusIndex - 1);
      }
      return true;
    }

    // === 선택 ===
    if (e.type == UiEventType::KeyDown) {

      // if (_focusIndex == 0) {
      //   ctx.store.lastDialog = DialogResult::Ok;
      // } else {
      //   ctx.store.lastDialog = DialogResult::Cancel;
      // }
      // ctx.commands.post(UiCommand::Dismiss());

      // return true;
  
      if (e.data.key.key == Key::Ok) {
        ctx.store.lastDialog = DialogResult::Ok;
        ctx.store.bump();
        ctx.commands.post(UiCommand::Dismiss());
        return true;
      }
      if (!_confirmOnly && e.data.key.key == Key::Back) {
        ctx.store.lastDialog = DialogResult::Cancel;
        ctx.store.bump();
        ctx.commands.post(UiCommand::Dismiss());
        return true;
      }
    }

    // === 긴 누름 Cancel ===
    if (e.type == UiEventType::KeyLongPress) {
      ctx.store.lastDialog = DialogResult::Cancel;
      ctx.commands.post(UiCommand::Dismiss());
      return true;
    }

    // 모든 입력 소비
    return true;
  }

  void draw(const UiContext&, IDisplay& d) override {

    int w = d.width();
    int h = d.height();

    int boxW = w - 40;
    int boxH = 90;
    int x = 20;
    int y = (h - boxH) / 2;

    // 배경/테두리
    d.fillRect(x, y, boxW, boxH, 0x0000);
    d.drawRect(x, y, boxW, boxH, 0xFFFF);

    FontSpec f;
    f.size = FontSize::SMALL;

    // 제목
    d.drawText(x + 8, y + 8, _title, 0xFFE0, 0x0000, f);

    // 본문
    d.drawText(x + 8, y + 28, _message, 0xFFFF, 0x0000, f);

    // 버튼 영역
    int btnY = y + boxH - 22;
    int okX = x + 16;
    int cancelX = x + boxW - 80;

    // OK
    if (_focusIndex == 0) {
      // 포커스 강조
      d.fillRect(okX - 4, btnY - 4, 56, 18, 0x07E0);
      d.drawText(okX, btnY, " OK ", 0x0000, 0x07E0, f);
    } else {
      d.drawText(okX, btnY, " OK ", 0xFFFF, 0x0000, f);
    }

    // Cancel (있을 때)
    if (_maxIndex > 0) {
      if (_focusIndex == 1) {
        d.fillRect(cancelX - 4, btnY - 4, 80, 18, 0xF800);
        d.drawText(cancelX, btnY, " Cancel ", 0x0000, 0xF800, f);
      } else {
        d.drawText(cancelX, btnY, " Cancel ", 0xFFFF, 0x0000, f);
      }
    }
  }


private:
  const char* _title;
  const char* _message;
  bool _confirmOnly;

  int _focusIndex = 0;
  int _maxIndex   = 1;
};
