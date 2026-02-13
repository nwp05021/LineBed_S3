#pragma once
#include "../../core/fsm/StateMachine.h"
#include "../../core/event/IEventSink.h"

namespace domain::network {

class NetworkManager {
public:
    enum class State : uint8_t {
        Disconnected,
        Connecting,
        Connected
    };

    explicit NetworkManager(core::event::IEventSink& sink);

    void begin();
    void tick100ms();
    void tick1s();
    void handleEvent(const core::event::Event& e);

    State state() const { return _sm.state(); }

    // Public API for App/UI (no direct state writes)
    void requestWifiConnect();

private:
    struct Reducer {
        core::event::IEventSink* sink {};
        State operator()(State s, const core::event::Event& e) const;
    };

    core::event::IEventSink& _sink;
    core::fsm::StateMachine<State, Reducer> _sm;
};

} // namespace domain::network
