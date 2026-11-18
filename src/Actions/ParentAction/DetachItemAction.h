#ifndef DETACHITEMACTION_H
#define DETACHITEMACTION_H

#include "../../Command/Command.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"

class DetachItemAction
{
  public:
    DetachItemAction(IOService& ioService, ListItemService& listItemService);
    void execute(Command& command, ListName& listName);

  private:
    IOService& ioService;
    ListItemService& listItemService;
};

#endif // DETACHITEMACTION_H
