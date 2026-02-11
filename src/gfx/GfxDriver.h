#pragma once
#include "../display/FontSpec.h"
#include "../display/IDisplay.h"

class GfxDriver {
public:
  explicit GfxDriver(IDisplay& display);

  bool begin();
  void tick();     // 입력/상태 업데이트용
  void render();   // 화면 그리기

private:
  IDisplay& _d;
  bool _dirty = true;
};
