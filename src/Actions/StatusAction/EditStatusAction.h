#ifndef EDITSTATUS_H
#define EDITSTATUS_H

#include "../../Command/Command.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"

class EditStatusAction
{
  public:
    EditStatusAction(IOService& ioService, ListItemService& listItemService);
    void execute(Command& command, ListName& listName, int statusNumber);
    void executeReset(Command& command, ListName& listName);

  private:
    IOService& ioService;
    ListItemService& listItemService;
};

#endif // EDITSTATUS_H
