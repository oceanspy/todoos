#ifndef TODOOS_EVENT_H
#define TODOOS_EVENT_H

#include <string>

class Event {
public:
    Event(std::string type, std::string payload);
    std::string getType();
    std::string getPayload();
private:
    std::string type;
    std::string payload;
};


#endif //TODOOS_EVENT_H
