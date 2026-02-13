#pragma once
#include <stddef.h>
#include "Event.h"

namespace core::event {

// Very small pub/sub bus using function pointers (embedded-friendly).
// Use when you want multiple listeners without App routing.
class EventBus {
public:
    using Handler = void (*)(const Event&);

    template <size_t MaxHandlers>
    class Fixed {
    public:
        bool subscribe(Handler h) {
            if (!h) return false;
            if (_count >= MaxHandlers) return false;
            _handlers[_count++] = h;
            return true;
        }

        void publish(const Event& e) const {
            for (size_t i = 0; i < _count; ++i) {
                _handlers[i](e);
            }
        }

    private:
        Handler _handlers[MaxHandlers] {};
        size_t  _count { 0 };
    };
};

} // namespace core::event
