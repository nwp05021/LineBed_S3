#pragma once
#include "Event.h"

namespace core::event {

class IEventSink {
public:
    virtual ~IEventSink() = default;
    virtual bool post(const Event& e) = 0;
};

} // namespace core::event
