#include "AppEvents.h"
#include "common/EventQueue.h"

static EventQueue<AppEvent, 8> queue;

void AppEvent_Push(const AppEvent& ev) {
    queue.push(ev);
}

bool AppEvent_Pop(AppEvent& ev) {
    return queue.pop(ev);
}
