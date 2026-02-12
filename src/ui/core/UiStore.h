//-----------------------------------------------
// UiStore.h (단일 상태 저장소)
//    - 센서값, 설정값, 연결상태(BLE/WiFi/Shadow), 현재 선택값 같은 “화면들이 같이 보는 데이터”
//-----------------------------------------------
#pragma once
#include <stdint.h>

enum class ConnState : uint8_t { Disconnected, Connecting, Connected, Error };
enum class SyncState : uint8_t { Idle, Pending, Synced, Failed };

enum class DialogResult {
  None,
  Ok,
  Cancel
};

struct UiStore {
  // Dashboard
  float airTempC = 0;
  float waterTempC = 0;
  uint8_t humidity = 0;

  // Connectivity
  ConnState wifi = ConnState::Disconnected;
  ConnState bleProv = ConnState::Disconnected;
  SyncState shadow = SyncState::Idle;

  // Settings preview/edit
  int setpointTempC = 22;
  bool pumpEnabled = true;

  // “store가 바뀌었는지” versioning
  uint32_t revision = 0;

  DialogResult lastDialog = DialogResult::None;

  void bump() { revision++; }
};
