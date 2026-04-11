#ifndef TODOOS_MOVE_H
#define TODOOS_MOVE_H

#include "../../Command/Command.h"
#include "../../Command/CommandService.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"
#include "../../List/ListService.h"

class Move
{
  public:
    Move(IOService& ioService,
         Command& command,
         CommandService& commandService,
         ListService& listService,
         ListItemService& listItemService);
    std::string make(ListName& listName);

  private:
    IOService& ioService;
    Command& command;
    CommandService& commandService;
    ListService& listService;
    ListItemService& listItemService;
};

#endif // TODOOS_MOVE_H
