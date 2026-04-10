#pragma once

#include "../CLIThemes/CLIThemeService.h"
#include "../Command/Command.h"
#include "../Config/ConfigService.h"
#include "../IOService/IOService.h"
#include "../List/ListItemService.h"
#include "../List/ListService.h"

class FindUseCase
{
  public:
    FindUseCase(IOService& ioService,
                Command& command,
                ConfigService& configService,
                ListService& listService,
                ListItemService& listItemService,
                CLIThemeService& cliThemeService);
    void execute();

  private:
    IOService& ioService;
    Command& command;
    ConfigService& configService;
    ListService& listService;
    ListItemService& listItemService;
    CLIThemeService& cliThemeService;
};
