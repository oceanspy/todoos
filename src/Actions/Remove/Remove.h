#ifndef REMOVE_H
#define REMOVE_H

#include "../../Command/Command.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"

class Remove
{
  public:
    Remove(IOService& ioService, Command& command, ListItemService& listItemService);
    void remove(ListName& listName);
    void restore(ListName& listName);
    void archive(ListName& listName);

  private:
    IOService& ioService;
    Command& command;
    ListItemService& listItemService;
};

#endif // REMOVE_H
