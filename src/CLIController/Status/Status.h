#ifndef STATUS_H
#define STATUS_H

#include "../../Command/Command.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"

class Status
{
  public:
    Status(IOService& ioService, Command& command, ListItemService& listItemService);
    void markAs(ListName& listName, int status);
    void set(ListName& listName);
    void reset(ListName& listName);

  private:
    IOService& ioService;
    Command& command;
    ListItemService& listItemService;
};

#endif // STATUS_H
