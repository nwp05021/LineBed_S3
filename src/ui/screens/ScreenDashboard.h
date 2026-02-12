#pragma once
#include "../core/Screen.h"
#include "../widgets/Label.h"
#include "../widgets/ValueRow.h"

class ScreenDashboard : public Screen {
public:
  void layout(const UiContext& ctx, int w, int h) override;
  bool handleEvent(const UiContext& ctx, const UiEvent& e) override;
};