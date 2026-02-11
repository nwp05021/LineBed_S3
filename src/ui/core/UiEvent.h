//-----------------------------------------------
// UiEvent.h (모든 입력/상태변화를 이벤트로 통일)
//    - EC11 회전/클릭, BLE 프로비저닝 상태변경, Shadow sync 결과 등
//-----------------------------------------------
#pragma once
#include <stdint.h>

enum class UiEventType : uint8_t {
  Tick,
  KeyDown,
  KeyUp,
  EncoderCW,
  EncoderCCW,

  // System events
  BleProvStateChanged,
  WifiStateChanged,
  ShadowSyncStateChanged,
};

enum class Key : uint8_t {
  Ok,
  Back,
};

struct UiEvent {
  UiEventType type;
  union {
    struct { Key key; } key;
    struct { int32_t delta; } encoder;
    struct { uint8_t state; } sys;   // 간단히 상태값만 (추후 구조체 확장)
  } data;
};
