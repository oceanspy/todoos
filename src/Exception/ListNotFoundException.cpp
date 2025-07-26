#include "ListNotFoundException.h"

ListNotFoundException::ListNotFoundException(const std::string& msg,
                                             const std::string& name,
                                             const std::string& variant)
  : message(msg)
  , name(name)
  , variant(variant)
{
}

std::string
ListNotFoundException::getMessage()
{
    return message;
}

std::string
ListNotFoundException::getName()
{
    return name;
}

std::string
ListNotFoundException::getVariant()
{
    return variant;
}
