#ifndef PRIORITY_H
#define PRIORITY_H

#include "../../Command/Command.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"

class Priority
{
  public:
    Priority(IOService& ioService, Command& command, ListItemService& listItemService);
    void increase(ListName& listName);
    void decrease(ListName& listName);
    void set(ListName& listName);

  private:
    IOService& ioService;
    Command& command;
    ListItemService& listItemService;
};

#endif // PRIORITY_H
