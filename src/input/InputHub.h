#pragma once
#include <vector>
#include <memory>
#include "InputAdapter.h"

class InputHub {
public:
  void add(std::unique_ptr<InputAdapter> a) {
    _adapters.emplace_back(std::move(a));
  }

  void begin() {
    for (auto& a : _adapters) a->begin();
  }

  bool poll(UiEvent& out) {
    for (auto& a : _adapters) {
      if (a->poll(out)) return true;
    }
    return false;
  }

private:
  std::vector<std::unique_ptr<InputAdapter>> _adapters;
};
