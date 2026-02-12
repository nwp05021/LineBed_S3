//-----------------------------------------------
// UiCommand.h
//
//-----------------------------------------------
#pragma once
#include <stdint.h>
#include "ScreenId.h"

enum class UiCommandType : uint8_t {
  None,
  PushScreen,
  ReplaceScreen,
  PopScreen,

  ShowToast,
  ShowDialog,
  DismissDialog,
};

struct UiCommand {
  UiCommandType type = UiCommandType::None;

  union {
    struct { ScreenId id; } nav;
    struct { const char* msg; uint16_t ms; } toast;
    struct { const char* title; const char* message; bool confirmOnly; } dialog;
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

  static UiCommand Dialog(const char* title, const char* msg, bool confirmOnly=false) {
    UiCommand c;
    c.type = UiCommandType::ShowDialog;
    c.data.dialog = { title, msg, confirmOnly };
    return c;
  }

  static UiCommand Dismiss() {
    UiCommand c;
    c.type = UiCommandType::DismissDialog;
    return c;
  }
};




