//-----------------------------------------------
// UiCommand.h
//
//-----------------------------------------------
#pragma once
#include <stdint.h>

enum class UiCommandType : uint8_t {
  None,
  PushScreen,
  ReplaceScreen,
  PopScreen,
  ShowToast,
};

enum class ScreenId : uint8_t {
  Dashboard,
  Settings,
  Provisioning,
  Debug,
};

struct UiCommand {
  UiCommandType type = UiCommandType::None;

  union {
    struct { ScreenId id; } nav;
    struct { const char* msg; uint16_t ms; } toast;
  } data;

  static UiCommand Push(ScreenId id) {
    UiCommand c; c.type = UiCommandType::PushScreen; c.data.nav.id = id; return c;
  }
  static UiCommand Replace(ScreenId id) {
    UiCommand c; c.type = UiCommandType::ReplaceScreen; c.data.nav.id = id; return c;
  }
  static UiCommand Pop() {
    UiCommand c; c.type = UiCommandType::PopScreen; return c;
  }
  static UiCommand Toast(const char* msg, uint16_t ms=1500) {
    UiCommand c; c.type = UiCommandType::ShowToast; c.data.toast.msg = msg; c.data.toast.ms = ms; return c;
  }
};
