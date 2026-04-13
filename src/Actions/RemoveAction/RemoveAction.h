#ifndef REMOVE_H
#define REMOVE_H

#include "../../Command/Command.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"

class RemoveAction
{
  public:
    RemoveAction(IOService& ioService, ListItemService& listItemService);
    void execute(Command& command, ListName& listName, const std::string& action);

  private:
    IOService& ioService;
    ListItemService& listItemService;
    void remove(Command& command, ListName& listName);
    void restore(Command& command, ListName& listName);
    void archive(Command& command, ListName& listName);
};

#endif // REMOVE_H
