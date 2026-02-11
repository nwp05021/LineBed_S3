#pragma once
#include <stdint.h>

enum class Dirty : uint16_t {
    None        = 0,
    Full        = 1 << 0,  // 화면 전체
    Header      = 1 << 1,
    Menu        = 1 << 2,
    Value       = 1 << 3,
    Footer      = 1 << 4
};

inline Dirty operator|(Dirty a, Dirty b) {
    return static_cast<Dirty>(static_cast<uint16_t>(a) | static_cast<uint16_t>(b));
}
inline bool hasDirty(Dirty a, Dirty b) {
    return (static_cast<uint16_t>(a) & static_cast<uint16_t>(b)) != 0;
}
