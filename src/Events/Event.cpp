#include "Event.h"

#include <utility>

Event::Event(std::string type, std::string payload)
  : type(std::move(type))
  , payload(std::move(payload))
{
}

std::string
Event::getType()
{
    return type;
}

std::string
Event::getPayload()
{
    return payload;
}
