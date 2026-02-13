#pragma once
#include "../core/fsm/StateMachine.h"
#include "../core/event/Event.h"
#include "../core/event/IEventSink.h"

namespace ui {

class UiManager {
public:
    enum class State : uint8_t {
        Splash,
        Main,
        Settings
    };

    explicit UiManager(core::event::IEventSink& sink);

    void begin();
    void tick100ms();
    void tick1s();
    void handleEvent(const core::event::Event& e);

    State state() const { return _sm.state(); }

    // Render entry point (call from App::loop when desired)
    void render();

private:
    struct Reducer {
        core::event::IEventSink* sink {};
        State operator()(State s, const core::event::Event& e) const;
    };

    core::event::IEventSink& _sink;
    core::fsm::StateMachine<State, Reducer> _sm;
    uint32_t _lastRenderMs { 0 };
};

} // namespace ui
