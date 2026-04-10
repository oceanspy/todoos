#pragma once

#include "../CLIThemes/CLIThemeService.h"
#include "../Command/Command.h"
#include "../Config/ConfigService.h"
#include "../IOService/IOService.h"
#include "../List/ListItemService.h"
#include "../List/ListService.h"

class StatusUseCase
{
  public:
    StatusUseCase(IOService& ioService,
                  Command& command,
                  ListItemService& listItemService,
                  ListService& listService,
                  ConfigService& configService,
                  CLIThemeService& cliThemeService,
                  int statusNumber);
    void execute();

  private:
    IOService& ioService;
    Command& command;
    ListItemService& listItemService;
    ListService& listService;
    ConfigService& configService;
    CLIThemeService& cliThemeService;
    int statusNumber;
};
