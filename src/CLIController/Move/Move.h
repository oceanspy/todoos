#ifndef TODOOS_MOVE_H
#define TODOOS_MOVE_H

#include "../../Command/Command.h"
#include "../../Command/CommandService.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"

class Move
{
  public:
    Move(IOService& ioService,
         ConfigService& configService,
         Command& command,
         CommandService& commandService,
         ListItemService& listItemService);
    std::string make();

  private:
    IOService& ioService;
    ConfigService& configService;
    Command& command;
    CommandService& commandService;
    ListItemService& listItemService;
};

#endif // TODOOS_MOVE_H
