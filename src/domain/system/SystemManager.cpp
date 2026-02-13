#include "SystemManager.h"
#include "../../core/logging/Logger.h"

using core::event::Event;
using core::event::EventType;

namespace domain::system {

static const __FlashStringHelper* TAG = F("System");

SystemManager::SystemManager(core::event::IEventSink& sink)
: _sink(sink)
, _sm(State::Booting, Reducer{ &sink })
{}

void SystemManager::begin() {
    core::logging::Logger::log(core::logging::Level::Info, TAG, "begin()");
    _sink.post(Event::make(EventType::Boot));
}

void SystemManager::tick100ms() {
    // Keep periodic work here if needed
}

void SystemManager::tick1s() {
    // Keep periodic work here if needed
}

void SystemManager::handleEvent(const Event& e) {
    _sm.dispatch(e);
}

SystemManager::State SystemManager::Reducer::operator()(State s, const Event& e) const {
    switch (s) {
        case State::Booting:
            if (e.type == EventType::Boot) {
                // Boot sequence done -> Running
                return State::Running;
            }
            return s;

        case State::Running:
            if (e.type == EventType::ErrorRaised) return State::Error;
            return s;

        case State::Error:
            if (e.type == EventType::ErrorCleared) return State::Running;
            return s;

        default:
            return s;
    }
}

} // namespace domain::system
