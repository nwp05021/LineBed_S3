#pragma once
#include "AppState.h"
#include "ui/UiStub.h"

class App {
public:
    void begin();
    void loop();

private:
    AppState state;
    UiStub ui;
};
