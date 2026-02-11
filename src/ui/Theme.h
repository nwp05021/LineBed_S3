#pragma once
#include "ThemeDefault.h"
#include "ThemeFont.h"

class Theme {
public:
  static const ThemePalette& colors() {
    return ThemeDark;
  }

  static FontSpec font(UIFont f) {
    return resolveFont(f);
  }
};
