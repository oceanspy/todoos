#ifndef MOVEITEMACTION_H
#define MOVEITEMACTION_H

#include "../../Command/Command.h"
#include "../../Command/CommandService.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"
#include "../../List/ListName.h"
#include "../../List/ListService.h"

class MoveItemAction
{
  public:
    MoveItemAction(IOService& ioService, ListService& listService, ListItemService& listItemService);
    std::string execute(Command& command, ListName& listName);

  private:
    IOService& ioService;
    ListService& listService;
    ListItemService& listItemService;
};

#endif // MOVEITEMACTION_H
