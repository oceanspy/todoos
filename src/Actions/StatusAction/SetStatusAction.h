#ifndef SETSTATUS_H
#define SETSTATUS_H

#include "../../Command/Command.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"

class SetStatusAction
{
  public:
    SetStatusAction(IOService& ioService, ListItemService& listItemService);
    void execute(Command& command, ListName& listName);

  private:
    IOService& ioService;
    ListItemService& listItemService;
};

#endif // SETSTATUS_H
