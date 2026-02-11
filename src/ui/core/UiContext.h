//-----------------------------------------------
// UiContext.h
//
//-----------------------------------------------
#pragma once
#include "UiStore.h"
#include "UiCommand.h"

class UiCommandQueue {
public:
  virtual ~UiCommandQueue() = default;
  virtual void post(const UiCommand& cmd) = 0;
};

struct UiContext {
  UiStore& store;
  UiCommandQueue& commands;
};
