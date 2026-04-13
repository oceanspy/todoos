#ifndef PRIORITY_H
#define PRIORITY_H

#include "../../Command/Command.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"

class PriorityAction
{
  public:
    PriorityAction(IOService& ioService, ListItemService& listItemService);
    void execute(Command& command, ListName& listName, const std::string& action);

  private:
    IOService& ioService;
    ListItemService& listItemService;
    void increase(Command& command, ListName& listName);
    void decrease(Command& command, ListName& listName);
    void set(Command& command, ListName& listName);
};

#endif // PRIORITY_H
