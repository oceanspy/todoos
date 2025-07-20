#ifndef PRIORITY_H
#define PRIORITY_H

#include "../../Command/Command.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"

class Priority
{
  public:
    Priority(IOService& ioService, Command& command, ListItemService& listItemService);
    void increase();
    void decrease();
    void set();

  private:
    IOService& ioService;
    Command& command;
    ListItemService& listItemService;
};

#endif // PRIORITY_H
