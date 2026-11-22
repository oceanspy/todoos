#ifndef REASSIGNITEMACTION_H
#define REASSIGNITEMACTION_H

#include "../../Command/Command.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"

class ReassignItemAction
{
  public:
    ReassignItemAction(IOService& ioService, ListItemService& listItemService);
    void execute(Command& command, ListName& listName);

  private:
    IOService& ioService;
    ListItemService& listItemService;
};

#endif // REASSIGNITEMACTION_H
