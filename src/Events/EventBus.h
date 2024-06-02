#ifndef TODOOS_EVENTBUS_H
#define TODOOS_EVENTBUS_H

#include <map>
#include <vector>
#include <string>
#include <functional>
#include "Event.h"

class EventBus {
public:
    void subscribe(const std::string& eventType, const std::function<void(Event)>& listener);
    void publish(Event event);
private:
    std::map<std::string, std::vector<std::function<void(Event)>>> listeners;
};


#endif //TODOOS_EVENTBUS_H
