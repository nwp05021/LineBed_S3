#include "UiManager.h"
#include "../core/logging/Logger.h"
#include <Arduino.h>

using core::event::Event;
using core::event::EventType;

namespace ui {

static const __FlashStringHelper* TAG = F("UI");

UiManager::UiManager(core::event::IEventSink& sink)
: _sink(sink)
, _sm(State::Splash, Reducer{ &sink })
{}

void UiManager::begin() {
    core::logging::Logger::log(core::logging::Level::Info, TAG, "begin()");
}

void UiManager::tick100ms() {}
void UiManager::tick1s() {}

void UiManager::handleEvent(const Event& e) {
    _sm.dispatch(e);
}

void UiManager::render() {
    // Foundation-only: render to Serial.
    // Replace with real display rendering later (infrastructure/display).
    const uint32_t now = millis();
    if (now - _lastRenderMs < 250) return; // throttle
    _lastRenderMs = now;

    Serial.print("[UI] state=");
    Serial.println((int)_sm.state());
}

UiManager::State UiManager::Reducer::operator()(State s, const Event& e) const {
    switch (s) {
        case State::Splash:
            if (e.type == EventType::Boot) return State::Main;
            return s;

        case State::Main:
            if (e.type == EventType::ButtonPressed && e.a == 1) return State::Settings;
            return s;

        case State::Settings:
            if (e.type == EventType::ButtonPressed && e.a == 1) return State::Main;
            return s;

        default:
            return s;
    }
}

} // namespace ui
