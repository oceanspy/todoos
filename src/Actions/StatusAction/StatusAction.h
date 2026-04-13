#ifndef STATUS_H
#define STATUS_H

#include "../../Command/Command.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"

class StatusAction
{
  public:
    StatusAction(IOService& ioService, ListItemService& listItemService);
    void execute(Command& command, ListName& listName, int statusNumber);
    void executeReset(Command& command, ListName& listName);

  private:
    IOService& ioService;
    ListItemService& listItemService;
};

#endif // STATUS_H
