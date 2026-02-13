#pragma once
#include <stdint.h>

namespace core::event {

// Keep Event small and POD-friendly for embedded.
enum class EventType : uint16_t {
    None = 0,

    // System lifecycle
    Boot,
    Tick1s,
    Tick100ms,

    // Input
    ButtonPressed,
    ButtonReleased,
    EncoderTurned,

    // Network
    WifiConnectRequested,
    WifiConnected,
    WifiDisconnected,

    // Cloud
    CloudConnected,
    CloudDisconnected,
    CloudCommand,

    // Device
    SensorUpdated,
    ActuatorCommand,

    // Errors
    ErrorRaised,
    ErrorCleared,
};

struct Event final {
    EventType type { EventType::None };

    // Generic numeric payload. Interpret based on `type`.
    // (Example: button id, encoder delta, error code, etc.)
    int32_t   a { 0 };
    int32_t   b { 0 };
    uint32_t  u { 0 };

    static constexpr Event make(EventType t, int32_t a0 = 0, int32_t b0 = 0, uint32_t u0 = 0) {
        return Event{ t, a0, b0, u0 };
    }
};

} // namespace core::event
