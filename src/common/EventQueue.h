#pragma once
#include <stdint.h>

template<typename T, uint8_t N>
class EventQueue {
public:
    bool push(const T& item) {
        if (count >= N) return false;
        buffer[tail] = item;
        tail = (tail + 1) % N;
        count++;
        return true;
    }

    bool pop(T& out) {
        if (count == 0) return false;
        out = buffer[head];
        head = (head + 1) % N;
        count--;
        return true;
    }

private:
    T buffer[N];
    uint8_t head = 0;
    uint8_t tail = 0;
    uint8_t count = 0;
};
