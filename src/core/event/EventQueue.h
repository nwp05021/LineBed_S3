#pragma once
#include <cstddef>   // ⭐ 추가
#include <stdint.h>
#include "Event.h"

namespace core::event {

// Lock-free single-producer/single-consumer ring buffer.
// If you need ISR -> main thread, consider adding critical sections.
template <size_t Capacity>
class EventQueue {
public:
    bool push(const Event& e) {
        const uint16_t next = (uint16_t)((_head + 1) % Capacity);
        if (next == _tail) return false; // full
        _buf[_head] = e;
        _head = next;
        return true;
    }

    bool pop(Event& out) {
        if (_tail == _head) return false; // empty
        out = _buf[_tail];
        _tail = (uint16_t)((_tail + 1) % Capacity);
        return true;
    }

    bool isEmpty() const { return _tail == _head; }

    void clear() { _head = _tail = 0; }

private:
    Event _buf[Capacity] {};
    volatile uint16_t _head { 0 };
    volatile uint16_t _tail { 0 };
};

} // namespace core::event
