#pragma once
#include <Arduino.h>
#include "ui/InputEvent.h"

// EC11 (A/B + SW) 기본 드라이버
// - 인터럽트 기반(회전), 디바운스(스위치)
// - 핀은 보드에 맞게 수정하세요.
class EncoderEc11 {
public:
  void begin();
  bool poll(InputEvent& out);

private:
  static void IRAM_ATTR isrA();
  static void IRAM_ATTR isrB();

  static volatile int32_t sDelta;
  static constexpr int PIN_A = 6;
  static constexpr int PIN_B = 5;
  static constexpr int PIN_SW = 4;

  // 버튼 디바운스/롱클릭
  bool _btnDown = false;
  uint32_t _btnDownMs = 0;
  uint32_t _lastBtnChangeMs = 0;

  // 이벤트 큐(간단)
  int32_t _pendingRotate = 0;
  bool _pendingClick = false;
  bool _pendingLong = false;
};
