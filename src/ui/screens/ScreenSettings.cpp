#include "ScreenSettings.h"

void ScreenSettings::layout(const UiContext& ctx, int w, int h) {
  _widgets.clear();

  _widgets.emplace_back(std::make_unique<Label>("SETTINGS"));
  _widgets.back()->setBounds({8, 6, (int16_t)(w-16), 16});

  // Store 포인터 연결 (실전에서는 getter/formatter로 더 깔끔하게)
  // 여기서는 “개념 스켈레톤”
  static float dummyAir = 25.3f;
  static float dummyWater = 21.8f;

  _widgets.emplace_back(std::make_unique<ValueRow>("Air", &dummyAir));
  _widgets.back()->setBounds({8, 30, (int16_t)(w-16), 26});

  _widgets.emplace_back(std::make_unique<ValueRow>("Water", &dummyWater));
  _widgets.back()->setBounds({8, 60, (int16_t)(w-16), 26});

  setFocus(1);
}

bool ScreenSettings::handleEvent(const UiContext& ctx, const UiEvent& e) {
  switch (e.type) {
    case UiEventType::EncoderCW:
      _focus.next();
      _dirty = true;
      return true;

    case UiEventType::EncoderCCW:
      _focus.prev();
      _dirty = true;
      return true;

    case UiEventType::KeyDown:
      // 값 편집 진입
      //enterEditMode();
      return true;

    case UiEventType::KeyLongPress:
      // 🔥 길게 = 저장 확인 다이얼로그
      ctx.commands.post(
        UiCommand::Dialog("Save", "Apply changes?", false)
      );
      return true;

    case UiEventType::KeyDoubleClick:
      // 🔥 두 번 = 즉시 저장
      //applyAndExit();
      ctx.commands.post(UiCommand::Toast("Saved", 1000));
      return true;

    default:
      break;
  }
  return Screen::handleEvent(ctx, e);
}

void ScreenSettings::onStoreChanged(const UiContext& ctx) {
  if (ctx.store.lastDialog == DialogResult::Ok) {
    //applyAndExit();
    ctx.commands.post(UiCommand::Toast("Applied", 1200));
  }
  ctx.store.lastDialog = DialogResult::None;
  Screen::onStoreChanged(ctx);
}
