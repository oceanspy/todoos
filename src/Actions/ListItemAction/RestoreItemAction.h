#ifndef RESTOREITEMACTION_H
#define RESTOREITEMACTION_H

#include "../../Command/Command.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"

class RestoreItemAction
{
  public:
    RestoreItemAction(IOService& ioService, ListItemService& listItemService);
    void execute(Command& command, ListName& listName);

  private:
    IOService& ioService;
    ListItemService& listItemService;
};

#endif // RESTOREITEMACTION_H
