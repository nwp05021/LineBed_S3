#include "display/DisplayLGFX_ST7789.h"
#include "ui/core/ScreenManager.h"
#include "ui/AppScreenFactory.h"
#include "gfx/GfxDriver.h"

#include "input/InputHub.h"
#include "input/EC11Adapter.h"
#include "input/ButtonAdapter.h"

DisplayLGFX_ST7789 display;
UiStore store;
AppScreenFactory factory;
ScreenManager ui(factory, store);

InputHub inputs;

void setup() {
  Serial.begin(115200);
  display.begin();
  display.setBrightness(200);

  ui.init(display, ScreenId::Dashboard);

  inputs.add(std::make_unique<EC11Adapter>(4, 5, 6));
  //inputs.add(std::make_unique<ButtonAdapter>(BACK_PIN, Key::Back));
  inputs.begin();
}

void loop() {
  // 1) 입력 → 이벤트 (Tick)
  UiEvent e{UiEventType::Tick};

  if (inputs.poll(e)) {
    ui.dispatch(e);
  } else {
    UiEvent tick{UiEventType::Tick};
    ui.dispatch(tick);
  }

  // 2) 그리기
  ui.render(display);

  delay(20);
}
