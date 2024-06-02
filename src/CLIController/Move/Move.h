#ifndef TODOOS_MOVE_H
#define TODOOS_MOVE_H

#include "../../List/ListItemService.h"
#include "../../IOService/IOService.h"
#include "../../Command/Command.h"
#include "../../Command/CommandService.h"

class Move {
public:
    Move(IOService& ioService, ConfigService& configService, Command& command, CommandService& commandService, ListItemService& listItemService);
    std::string make();
private:
    IOService& ioService;
    ConfigService& configService;
    Command& command;
    CommandService& commandService;
    ListItemService& listItemService;
};


#endif //TODOOS_MOVE_H
