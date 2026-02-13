#include "App.h"
#include <Arduino.h>
#include "../core/logging/Logger.h"

using core::event::Event;
using core::event::EventType;

namespace app {

static const __FlashStringHelper* TAG = F("App");

void App::begin() {
    Serial.begin(115200);
    delay(200);

    core::logging::Logger::log(core::logging::Level::Info, TAG, "begin()");

    _system.begin();
    _network.begin();
    _device.begin();
    _cloud.begin();
    _ui.begin();

    // Kickstart boot event (also posted by SystemManager::begin, but this is okay)
    post(Event::make(EventType::Boot));
}

bool App::post(const Event& e) {
    const bool ok = _queue.push(e);
    if (!ok) {
        // Queue full: raise an error event (best-effort)
        // Avoid recursion if even that fails
    }
    return ok;
}

void App::loop() {
    tickTimers();

    Event e;
    while (_queue.pop(e)) {
        dispatch(e);
    }

    // Render UI after processing events
    _ui.render();
}

void App::tickTimers() {
    const uint32_t now = millis();
    if (now - _last100ms >= 100) {
        _last100ms = now;
        const Event tick = Event::make(EventType::Tick100ms);
        dispatch(tick);
    }
    if (now - _last1s >= 1000) {
        _last1s = now;
        const Event tick = Event::make(EventType::Tick1s);
        dispatch(tick);
    }
}

void App::dispatch(const Event& e) {
    // App routes events to all managers (simple, predictable, stable).
    _system.handleEvent(e);
    _network.handleEvent(e);
    _device.handleEvent(e);
    _cloud.handleEvent(e);
    _ui.handleEvent(e);

    // Tick hooks (optional - if you want periodic functions rather than Tick events)
    if (e.type == EventType::Tick100ms) {
        _system.tick100ms();
        _network.tick100ms();
        _device.tick100ms();
        _cloud.tick100ms();
        _ui.tick100ms();
    } else if (e.type == EventType::Tick1s) {
        _system.tick1s();
        _network.tick1s();
        _device.tick1s();
        _cloud.tick1s();
        _ui.tick1s();
    }
}

} // namespace app
