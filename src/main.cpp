#include <Arduino.h>
#include "app/App.h"

static app::App g_app;

void setup() {
    g_app.begin();
}

void loop() {
    g_app.loop();
    delay(1); // yield
}
