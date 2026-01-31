#include "EncoderEc11.h"
#include <Arduino.h>

// =======================================
// EC11 Quadrature Encoder (Stable, Final)
// =======================================

// 이전 A/B 상태
static uint8_t sPrevAB = 0;
// 상태 누적 (4 transition = 1 detent)
static int8_t  sStepAccum = 0;

// 쿼드러처 상태 전이 테이블
// index = (prev << 2) | curr
static const int8_t kQuadTable[16] = {
   0, -1, +1,  0,
  +1,  0,  0, -1,
  -1,  0,  0, +1,
   0, +1, -1,  0
};

// -------------------------------------------------
// begin() : 반드시 App.cpp에서 호출됨
// -------------------------------------------------
void EncoderEc11::begin() {
  pinMode(PIN_A, INPUT_PULLUP);
  pinMode(PIN_B, INPUT_PULLUP);
  pinMode(PIN_SW, INPUT_PULLUP);

  // 초기 A/B 상태 캡처
  uint8_t a = digitalRead(PIN_A);
  uint8_t b = digitalRead(PIN_B);
  sPrevAB = (a << 1) | b;

  sStepAccum = 0;
}

// -------------------------------------------------
// poll() : 회전 / 클릭 이벤트 생성
// -------------------------------------------------
bool EncoderEc11::poll(InputEvent& ev) {
  ev.type = InputEventType::NONE;
  ev.delta = 0;

  // -----------------------------
  // 1. Read A/B pins
  // -----------------------------
  uint8_t a = digitalRead(PIN_A);
  uint8_t b = digitalRead(PIN_B);
  uint8_t ab = (a << 1) | b;

  // -----------------------------
  // 2. Quadrature decode
  // -----------------------------
  int8_t delta = kQuadTable[(sPrevAB << 2) | ab];
  sPrevAB = ab;

  if (delta != 0) {
    sStepAccum += delta;

    // 누적값 클램프 (폭주 방지)
    if (sStepAccum > 3)  sStepAccum = 3;
    if (sStepAccum < -3) sStepAccum = -3;

    // detent window 판정
    // 중앙 구간(-1~+1)은 detent 내부 → 아무 것도 안 함
    if (sStepAccum >= 2) {
      // 다음 detent로 확실히 넘어감
      sStepAccum = 0;
      ev.type  = InputEventType::ROTATE;
      ev.delta = +1;
      return true;
    }
    else if (sStepAccum <= -2) {
      sStepAccum = 0;
      ev.type  = InputEventType::ROTATE;
      ev.delta = -1;
      return true;
    }
  }

  // -----------------------------
  // 3. Button (SW) handling
  // -----------------------------
  static bool     btnPressed = false;
  static uint32_t btnPressMs = 0;
  static uint32_t lastBtnMs  = 0;

  uint32_t now = millis();
  bool sw = (digitalRead(PIN_SW) == LOW);

  if (now - lastBtnMs > 30) { // debounce
    if (sw && !btnPressed) {
      btnPressed = true;
      btnPressMs = now;
    }
    else if (!sw && btnPressed) {
      btnPressed = false;
      uint32_t dur = now - btnPressMs;
      if (dur > 600) {
        ev.type = InputEventType::LONG_CLICK;
        lastBtnMs = now;
        return true;
      }
      else {
        ev.type = InputEventType::CLICK;
        lastBtnMs = now;
        return true;
      }
    }
    lastBtnMs = now;
  }

  return false;
}