//-----------------------------------------------
// ScreenSettings.h
//
//-----------------------------------------------
#pragma once
#include "../core/Screen.h"
#include "../widgets/Label.h"
#include "../widgets/ValueRow.h"

class ScreenSettings : public Screen {
public:
  void layout(UiContext& ctx, int w, int h) override;
  bool handleEvent(UiContext& ctx, const UiEvent& e) override;
};