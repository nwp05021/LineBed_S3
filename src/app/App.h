#pragma once
#include <cstddef>   // ⭐ 추가
#include "../core/event/EventQueue.h"
#include "../core/event/IEventSink.h"

#include "../domain/system/SystemManager.h"
#include "../domain/network/NetworkManager.h"
#include "../domain/device/DeviceManager.h"
#include "../domain/cloud/CloudManager.h"
#include "../ui/UiManager.h"

namespace app {

class App : public core::event::IEventSink {
public:
    void begin();
    void loop();

    // IEventSink
    bool post(const core::event::Event& e) override;

private:
    // Central event queue for the whole firmware.
    static constexpr size_t QUEUE_CAPACITY = 32;
    core::event::EventQueue<QUEUE_CAPACITY> _queue;

    // Managers (each owns its own state)
    domain::system::SystemManager  _system { *this };
    domain::network::NetworkManager _network { *this };
    domain::device::DeviceManager  _device { *this };
    domain::cloud::CloudManager    _cloud { *this };
    ui::UiManager                  _ui { *this };

    // Timers
    uint32_t _last100ms { 0 };
    uint32_t _last1s { 0 };

    void dispatch(const core::event::Event& e);
    void tickTimers();
};

} // namespace app
