#pragma once
#include "InputAdapter.h"
#include <Arduino.h>

class ButtonAdapter final : public InputAdapter {
public:
  ButtonAdapter(int pin, Key key);

  void begin() override;
  bool poll(UiEvent& out) override;

private:
  int _pin;
  Key _key;
  uint32_t _lastMs = 0;
  static constexpr uint16_t DEBOUNCE_MS = 30;
};
