#include "ScreenDashboard.h"

void ScreenDashboard::layout(const UiContext& ctx, int w, int h) {
  _widgets.clear();

  _widgets.emplace_back(std::make_unique<Label>("DASHBOARD"));
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

bool ScreenDashboard::handleEvent(const UiContext& ctx, const UiEvent& e) {
  if (e.type == UiEventType::EncoderCW)  { focusNext(); return true; }
  if (e.type == UiEventType::EncoderCCW){ focusPrev(); return true; }

  if (e.type == UiEventType::KeyDown && e.data.key.key == Key::Ok) {
    ctx.commands.post(UiCommand::Push(ScreenId::Settings));
    return true;
  }
  return Screen::handleEvent(ctx, e);
}