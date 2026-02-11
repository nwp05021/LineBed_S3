#include <Arduino.h>
#include "App.h"

void App::begin() {
    ui.begin();
}

void App::loop() {
    ui.poll();
    ui.render(state);
    delay(10);
}
