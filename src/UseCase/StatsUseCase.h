#pragma once

#include "../CLIThemes/CLIThemeService.h"
#include "../Command/Command.h"
#include "../Config/ConfigService.h"
#include "../IOService/IOService.h"
#include "../List/ListItemService.h"
#include "../List/ListService.h"

class StatsUseCase
{
  public:
    StatsUseCase(IOService& ioService,
                 Command& command,
                 ConfigService& configService,
                 ListItemService& listItemService,
                 CLIThemeService& cliThemeService,
                 ListService& listService);
    void execute();

  private:
    IOService& ioService;
    Command& command;
    ConfigService& configService;
    ListItemService& listItemService;
    CLIThemeService& cliThemeService;
    ListService& listService;
};
