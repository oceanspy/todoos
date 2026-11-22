#ifndef ATTACHITEMACTION_H
#define ATTACHITEMACTION_H

#include "../../Command/Command.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"

class AttachItemAction
{
  public:
    AttachItemAction(IOService& ioService, ListItemService& listItemService);
    void execute(Command& command, ListName& listName);

  private:
    IOService& ioService;
    ListItemService& listItemService;
};

#endif // ATTACHITEMACTION_H
