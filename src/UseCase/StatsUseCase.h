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
                 ConfigService& configService,
                 ListItemService& listItemService,
                 ThemeService& themeService,
                 ListService& listService);
    void execute(Command& command, ListName& currentList);

  private:
    IOService& ioService;
    ConfigService& configService;
    ListItemService& listItemService;
    ThemeService& themeService;
    ListService& listService;
};
