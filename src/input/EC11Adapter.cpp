#include "EC11Adapter.h"

EC11Adapter::EC11Adapter(int pinA, int pinB, int pinSW)
  : _pinA(pinA), _pinB(pinB), _pinSW(pinSW) {}

void EC11Adapter::begin() {
  pinMode(_pinA, INPUT_PULLUP);
  pinMode(_pinB, INPUT_PULLUP);
  if (_pinSW >= 0) pinMode(_pinSW, INPUT_PULLUP);

  _prevAB = readAB();
}

int EC11Adapter::readAB() {
  int a = digitalRead(_pinA);
  int b = digitalRead(_pinB);
  return (a << 1) | b;
}

bool EC11Adapter::poll(UiEvent& out) {
  uint32_t now = millis();

  // ---- Encoder rotation ----
  int ab = readAB();
  if (ab != _prevAB) {
    uint8_t state = (_prevAB << 2) | ab;
    _prevAB = ab;

    // Gray code decode (EC11 standard)
    int8_t delta = 0;
    switch (state) {
      case 0b0001:
      case 0b0111:
      case 0b1110:
      case 0b1000: delta = +1; break;
      case 0b0010:
      case 0b0100:
      case 0b1101:
      case 0b1011: delta = -1; break;
      default: break;
    }

    if (delta != 0 && (now - _lastMoveMs) >= DEBOUNCE_MS) {
      _lastMoveMs = now;

      // 가속: 빠르게 돌리면 2칸씩
      bool fast = (now - _lastMoveMs) < FAST_TURN_MS;
      int step = fast ? 2 : 1;

      out.type = (delta > 0) ? UiEventType::EncoderCW
                             : UiEventType::EncoderCCW;
      out.data.encoder.delta = step;
      return true;
    }
  }

  // ---- Button (SW) ----
  if (_pinSW >= 0) {
    bool keyDown = (digitalRead(_pinSW) == LOW);
    uint32_t now = millis();

    // Button down
    if (keyDown && !_btnPressed) {
      _btnPressed = true;
      _btnDownMs = now;
    }

    // Button held (long press)
    if (keyDown && _btnPressed) {
      if ((now - _btnDownMs) >= LONG_PRESS_MS) {
        _btnPressed = false;
        _clickCount = 0;

        out.type = UiEventType::KeyLongPress;
        out.data.key.key = Key::Ok;
        return true;
      }
    }

    // Button released
    if (!keyDown && _btnPressed) {
      _btnPressed = false;
      _btnUpMs = now;
      _clickCount++;
    }

    // Click evaluation
    if (_clickCount > 0 && (now - _btnUpMs) > DOUBLE_CLICK_MS) {
      if (_clickCount == 1) {
        out.type = UiEventType::KeyDown;
        out.data.key.key = Key::Ok;
      } else if (_clickCount >= 2) {
        out.type = UiEventType::KeyDoubleClick;
        out.data.key.key = Key::Ok;
      }
      _clickCount = 0;
      return true;
    }
  }
}