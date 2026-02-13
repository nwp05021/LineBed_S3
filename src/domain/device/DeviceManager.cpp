#include "DeviceManager.h"
#include "../../core/logging/Logger.h"

using core::event::Event;
using core::event::EventType;

namespace domain::device {

static const __FlashStringHelper* TAG = F("Device");

DeviceManager::DeviceManager(core::event::IEventSink& sink)
: _sink(sink)
, _sm(State::Idle, Reducer{ &sink })
{}

void DeviceManager::begin() {
    core::logging::Logger::log(core::logging::Level::Info, TAG, "begin()");
    // init sensors/actuators here (later under infrastructure/)
}

void DeviceManager::tick100ms() {
    // In real integration: sample sensors, debounce, etc.
    // Example: post sensor update every 100ms if needed.
    // _sink.post(Event::make(EventType::SensorUpdated, value1, value2, flags));
}

void DeviceManager::tick1s() {
    // slow tasks
}

void DeviceManager::handleEvent(const Event& e) {
    _sm.dispatch(e);
}

DeviceManager::State DeviceManager::Reducer::operator()(State s, const Event& e) const {
    switch (s) {
        case State::Idle:
            if (e.type == EventType::ActuatorCommand) return State::Active;
            if (e.type == EventType::ErrorRaised) return State::Fault;
            return s;

        case State::Active:
            if (e.type == EventType::ActuatorCommand && e.a == 0) return State::Idle; // example
            if (e.type == EventType::ErrorRaised) return State::Fault;
            return s;

        case State::Fault:
            if (e.type == EventType::ErrorCleared) return State::Idle;
            return s;

        default:
            return s;
    }
}

} // namespace domain::device
