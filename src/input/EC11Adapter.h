#pragma once
#include "InputAdapter.h"
#include <Arduino.h>

class EC11Adapter final : public InputAdapter {
public:
  EC11Adapter(int pinA, int pinB, int pinSW = -1);

  void begin() override;
  bool poll(UiEvent& out) override;

private:
  int _pinA, _pinB, _pinSW;

  // encoder state
  uint8_t _prevAB = 0;
  int32_t _accum = 0;

  // timing
  uint32_t _lastMoveMs = 0;
  uint32_t _lastBtnMs  = 0;

  // config
  static constexpr uint16_t DEBOUNCE_MS = 5;
  static constexpr uint16_t BTN_DEBOUNCE_MS = 30;
  static constexpr uint16_t FAST_TURN_MS = 80; // 가속 기준

  int readAB();

  // 상태 변수 추가
  bool _btnPressed = false;
  uint32_t _btnDownMs = 0;
  uint32_t _btnUpMs = 0;
  uint8_t _clickCount = 0;

  static constexpr uint16_t LONG_PRESS_MS = 700;
  static constexpr uint16_t DOUBLE_CLICK_MS = 300;  
};
