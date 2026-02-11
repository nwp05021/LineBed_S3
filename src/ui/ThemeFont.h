#pragma once
#include "display/FontSpec.h"

enum class UIFont : uint8_t {
  Label,
  Value,
  Title,
  Korean
};

inline FontSpec resolveFont(UIFont f) {
  switch (f) {
    case UIFont::Value:   return FontSpec::Value();
    case UIFont::Title:   return FontSpec::Title();
    case UIFont::Korean:  return FontSpec::Korean();
    case UIFont::Label:
    default:              return FontSpec::Label();
  }
}
