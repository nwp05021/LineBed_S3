//-----------------------------------------------
// DirtyRect.h
//
// “더럽혀진 영역”을 논리적으로 계산
// 디스플레이 범위로 클리핑
//-----------------------------------------------
#pragma once
#include <stdint.h>
#include <algorithm>

struct Rect {
  int16_t x, y, w, h;

  bool empty() const {
    return w <= 0 || h <= 0;
  }
};

class DirtyRect {
public:
  void reset() {
    _rect = {0, 0, 0, 0};
  }

  void merge(int x, int y, int w, int h) {
    if (w <= 0 || h <= 0) return;

    if (_rect.empty()) {
      _rect = { (int16_t)x, (int16_t)y,
                (int16_t)w, (int16_t)h };
      return;
    }

    int x2 = std::min(_rect.x + _rect.w, x + w);
    int y2 = std::min(_rect.y + _rect.h, y + h);

    _rect.x = std::min(_rect.x, (int16_t)x);
    _rect.y = std::min(_rect.y, (int16_t)y);
    _rect.w = x2 - _rect.x;
    _rect.h = y2 - _rect.y;
  }

  // ✅ 핵심 추가
  void clip(int16_t maxW, int16_t maxH) {
    if (_rect.empty()) return;

    int16_t x1 = std::max<int16_t>(0, _rect.x);
    int16_t y1 = std::max<int16_t>(0, _rect.y);

    int16_t x2 = std::min<int16_t>(maxW, _rect.x + _rect.w);
    int16_t y2 = std::min<int16_t>(maxH, _rect.y + _rect.h);

    _rect.x = x1;
    _rect.y = y1;
    _rect.w = x2 - x1;
    _rect.h = y2 - y1;

    if (_rect.w <= 0 || _rect.h <= 0) {
      reset(); // 화면 밖이면 무효
    }
  }

  const Rect& rect() const { return _rect; }

private:
  Rect _rect {0, 0, 0, 0};
};
