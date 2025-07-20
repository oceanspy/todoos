#include "EventBus.h"

void
EventBus::subscribe(const std::string& eventType, const std::function<void(Event)>& listener)
{
    listeners[eventType].push_back(listener);
}

void
EventBus::publish(Event event)
{
    if (listeners.count(event.getType()) > 0) {
        for (auto& listener : listeners[event.getType()]) {
            listener(event);
        }
    }
}
