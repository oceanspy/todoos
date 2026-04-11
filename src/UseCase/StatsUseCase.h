#pragma once

#include "../Command/Command.h"
#include "../Config/ConfigService.h"
#include "../IOService/IOService.h"
#include "../List/ListItemService.h"
#include "../List/ListService.h"
#include "../Themes/ThemeService.h"

class StatsUseCase
{
  public:
    StatsUseCase(IOService& ioService,
                 Command& command,
                 ConfigService& configService,
                 ListItemService& listItemService,
                 ThemeService& themeService,
                 ListService& listService);
    void execute();

  private:
    IOService& ioService;
    Command& command;
    ConfigService& configService;
    ListItemService& listItemService;
    ThemeService& themeService;
    ListService& listService;
};
