#pragma once
#include "core/ScreenManager.h"
#include "screens/ScreenDashboard.h"
#include "screens/ScreenSettings.h"

class AppScreenFactory : public ScreenFactory {
public:
  std::unique_ptr<Screen> create(ScreenId id) override {
    switch (id) {
      case ScreenId::Dashboard: return std::make_unique<ScreenDashboard>();
      case ScreenId::Settings:  return std::make_unique<ScreenSettings>();
      default: return std::make_unique<ScreenDashboard>();
    }
  }
};
