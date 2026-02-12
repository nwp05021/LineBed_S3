#pragma once
#include "../ui/core/UiEvent.h"

class InputAdapter {
public:
  virtual ~InputAdapter() = default;
  virtual void begin() = 0;
  virtual bool poll(UiEvent& outEvent) = 0; // 이벤트 있으면 true
};
