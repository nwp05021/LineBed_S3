#include "NetworkManager.h"
#include "../../core/logging/Logger.h"

using core::event::Event;
using core::event::EventType;

namespace domain::network {

static const __FlashStringHelper* TAG = F("Network");

NetworkManager::NetworkManager(core::event::IEventSink& sink)
: _sink(sink)
, _sm(State::Disconnected, Reducer{ &sink })
{}

void NetworkManager::begin() {
    core::logging::Logger::log(core::logging::Level::Info, TAG, "begin()");
    // In real integration: initialize WiFi stack
}

void NetworkManager::tick100ms() {
    // In real integration: poll WiFi driver, timeouts, etc.
}

void NetworkManager::tick1s() {
    // Optional keepalive
}

void NetworkManager::requestWifiConnect() {
    _sink.post(Event::make(EventType::WifiConnectRequested));
}

void NetworkManager::handleEvent(const Event& e) {
    _sm.dispatch(e);
}

NetworkManager::State NetworkManager::Reducer::operator()(State s, const Event& e) const {
    switch (s) {
        case State::Disconnected:
            if (e.type == EventType::WifiConnectRequested) {
                // In real integration: start connection attempt
                return State::Connecting;
            }
            return s;

        case State::Connecting:
            if (e.type == EventType::WifiConnected) return State::Connected;
            if (e.type == EventType::WifiDisconnected) return State::Disconnected;
            return s;

        case State::Connected:
            if (e.type == EventType::WifiDisconnected) return State::Disconnected;
            return s;

        default:
            return s;
    }
}

} // namespace domain::network
