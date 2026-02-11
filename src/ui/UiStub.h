#pragma once
#include "app/AppState.h"

class UiStub {
public:
    void begin() {}
    void poll() {}
    void render(const AppState&) {}
};
