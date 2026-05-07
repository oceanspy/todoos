#include "ListItemNotFoundException.h"

ListItemNotFoundException::ListItemNotFoundException(const std::string msg,
                                                     const std::string id,
                                                     const ListName listName)
  : message(msg)
  , id(id)
  , listName(listName)
{
}

const char*
ListItemNotFoundException::what() const noexcept
{
    return message.c_str();
}

std::string&
ListItemNotFoundException::getMessage()
{
    return message;
}

std::string&
ListItemNotFoundException::getId()
{
    return id;
}

ListName&
ListItemNotFoundException::getListName()
{
    return listName;
}
