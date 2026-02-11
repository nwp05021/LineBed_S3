#pragma once
#include <vector>
#include "Widget.h"

class FocusManager {
public:
  void bind(const std::vector<std::unique_ptr<Widget>>* widgets) {
    _widgets = widgets;
    _index = -1;
  }

  int index() const { return _index; }

  void set(int idx) {
    if (!_widgets) return;
    if (_index == idx) return;

    if (_index >= 0) (*_widgets)[_index]->setFocused(false);
    _index = idx;
    if (_index >= 0) (*_widgets)[_index]->setFocused(true);
  }

  void next() { set(find(+1)); }
  void prev() { set(find(-1)); }

private:
  int find(int dir) {
    if (!_widgets || _widgets->empty()) return -1;

    int start = _index < 0 ? 0 : _index;
    for (int step=1; step <= (int)_widgets->size(); step++) {
      int i = (start + dir*step + (int)_widgets->size()) % (int)_widgets->size();
      if ((*_widgets)[i]->focusable()) return i;
    }
    return -1;
  }

  const std::vector<std::unique_ptr<Widget>>* _widgets = nullptr;
  int _index = -1;
};
