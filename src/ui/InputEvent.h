#pragma once
#include <stdint.h>

enum class InputEventType : uint8_t {
  NONE = 0,
  ROTATE,
  CLICK,
  LONG_CLICK
};

struct InputEvent {
  InputEventType type = InputEventType::NONE;
  int8_t delta = 0; // ROTATE: +1 / -1
};
