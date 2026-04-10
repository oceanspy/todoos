#pragma once

#include "../CLIThemes/CLIThemeService.h"
#include "../Command/Command.h"
#include "../Command/CommandService.h"
#include "../Config/ConfigService.h"
#include "../IOService/IOService.h"
#include "../List/ListItemService.h"
#include "../List/ListService.h"

class ListItemActionsUseCase
{
  public:
    ListItemActionsUseCase(IOService& ioService,
                           Command& command,
                           CommandService& commandService,
                           ListItemService& listItemService,
                           ListService& listService,
                           ConfigService& configService,
                           CLIThemeService& cliThemeService);
    void execute();

  private:
    IOService& ioService;
    Command& command;
    CommandService& commandService;
    ListItemService& listItemService;
    ListService& listService;
    ConfigService& configService;
    CLIThemeService& cliThemeService;
};
