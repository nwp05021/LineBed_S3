#pragma once
#include <stdint.h>

enum class FontType : uint8_t {
  ASCII,     // 내장 기본 폰트 (영문/기호)
  NUMERIC,   // 숫자 전용 (내장 or GFX)
  KOREAN     // VLW 한글
};

enum class FontSize : uint8_t {
  SMALL,
  MEDIUM,
  LARGE
};

enum class FontWeight : uint8_t {
  REGULAR,
  BOLD   // V1에서는 동일 폰트, V2에서 확장
};

struct FontSpec {
  FontType   type;
  FontSize   size;
  FontWeight weight;

  // ---- Presets (V1 표준) ----
  static FontSpec Label() {
    return { FontType::ASCII, FontSize::SMALL, FontWeight::REGULAR };
  }

  static FontSpec Value() {
    return { FontType::NUMERIC, FontSize::LARGE, FontWeight::REGULAR };
  }

  static FontSpec Korean() {
    return { FontType::KOREAN, FontSize::MEDIUM, FontWeight::REGULAR };
  }

  static FontSpec Title() {
    return { FontType::ASCII, FontSize::LARGE, FontWeight::BOLD };
  }
};