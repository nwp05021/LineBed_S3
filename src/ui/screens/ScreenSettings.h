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
  void layout(const UiContext& ctx, int w, int h) override;
  bool handleEvent(const UiContext& ctx, const UiEvent& e) override;
  void onStoreChanged(const UiContext& ctx) override;
};