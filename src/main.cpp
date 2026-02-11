#include "display/DisplayLGFX_ST7789.h"
#include "ui/core/ScreenManager.h"
#include "ui/AppScreenFactory.h"

DisplayLGFX_ST7789 display;
UiStore store;
AppScreenFactory factory;
ScreenManager ui(factory, store);

void setup() {
  Serial.begin(115200);
  display.begin();
  display.setBrightness(200);

  ui.init(display, ScreenId::Dashboard);
}

void loop() {
  // 1) 입력 → 이벤트 (임시: Tick만)
  UiEvent e{UiEventType::Tick};
  ui.dispatch(e);

  // 2) 필요시 렌더
  ui.renderIfNeeded(display);

  delay(20);
}
