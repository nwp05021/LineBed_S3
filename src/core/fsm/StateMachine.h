#pragma once
#include <stdint.h>
#include "../event/Event.h"

namespace core::fsm {

// Reducer-style state machine:
// - holds current State
// - updates via reducer(State, Event) -> State
//
// This is stable, testable, and doesn't force a big framework.
template <typename State, typename Reducer>
class StateMachine {
public:
    explicit StateMachine(State initial, Reducer reducer)
    : _state(initial), _reducer(reducer) {}

    State state() const { return _state; }

    void reset(State s) { _state = s; }

    void dispatch(const core::event::Event& e) {
        _state = _reducer(_state, e);
    }

private:
    State _state;
    Reducer _reducer;
};

} // namespace core::fsm
