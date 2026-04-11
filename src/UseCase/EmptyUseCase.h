#pragma once

#include "../Command/Command.h"
#include "../Config/ConfigService.h"
#include "../IOService/IOService.h"
#include "../List/ListItemService.h"
#include "../List/ListService.h"

class EmptyUseCase
{
  public:
    EmptyUseCase(IOService& ioService,
                 Command& command,
                 ListItemService& listItemService,
                 ListService& listService,
                 ConfigService& configService);
    void execute();

  private:
    IOService& ioService;
    Command& command;
    ListItemService& listItemService;
    ListService& listService;
    ConfigService& configService;
};
