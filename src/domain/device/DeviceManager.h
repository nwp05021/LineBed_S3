#pragma once
#include "../../core/fsm/StateMachine.h"
#include "../../core/event/IEventSink.h"

namespace domain::device {

class DeviceManager {
public:
    enum class State : uint8_t {
        Idle,
        Active,
        Fault
    };

    explicit DeviceManager(core::event::IEventSink& sink);

    void begin();
    void tick100ms();
    void tick1s();
    void handleEvent(const core::event::Event& e);

    State state() const { return _sm.state(); }

private:
    struct Reducer {
        core::event::IEventSink* sink {};
        State operator()(State s, const core::event::Event& e) const;
    };

    core::event::IEventSink& _sink;
    core::fsm::StateMachine<State, Reducer> _sm;
};

} // namespace domain::device
