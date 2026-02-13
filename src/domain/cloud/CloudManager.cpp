#include "CloudManager.h"
#include "../../core/logging/Logger.h"

using core::event::Event;
using core::event::EventType;

namespace domain::cloud {

static const __FlashStringHelper* TAG = F("Cloud");

CloudManager::CloudManager(core::event::IEventSink& sink)
: _sink(sink)
, _sm(State::Offline, Reducer{ &sink })
{}

void CloudManager::begin() {
    core::logging::Logger::log(core::logging::Level::Info, TAG, "begin()");
    // In real integration: init RainMaker/MQTT/etc.
}

void CloudManager::tick100ms() {}
void CloudManager::tick1s() {}

void CloudManager::handleEvent(const Event& e) {
    _sm.dispatch(e);
}

CloudManager::State CloudManager::Reducer::operator()(State s, const Event& e) const {
    switch (s) {
        case State::Offline:
            if (e.type == EventType::WifiConnected) return State::Connecting;
            return s;

        case State::Connecting:
            if (e.type == EventType::CloudConnected) return State::Online;
            if (e.type == EventType::WifiDisconnected) return State::Offline;
            return s;

        case State::Online:
            if (e.type == EventType::CloudDisconnected) return State::Connecting;
            if (e.type == EventType::WifiDisconnected) return State::Offline;
            return s;

        default:
            return s;
    }
}

} // namespace domain::cloud
