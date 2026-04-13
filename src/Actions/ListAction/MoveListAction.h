#ifndef TODOOS_MOVEACTION_H
#define TODOOS_MOVEACTION_H

#include "../../Command/Command.h"
#include "../../Command/CommandService.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"
#include "../../List/ListService.h"

class MoveListAction
{
  public:
    MoveListAction(IOService& ioService,
                   CommandService& commandService,
                   ListService& listService,
                   ListItemService& listItemService);
    std::string execute(Command& command, ListName& listName);

  private:
    IOService& ioService;
    CommandService& commandService;
    ListService& listService;
    ListItemService& listItemService;
};

#endif // TODOOS_MOVEACTION_H
