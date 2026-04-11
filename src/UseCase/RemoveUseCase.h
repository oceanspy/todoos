#pragma once

#include "../Command/Command.h"
#include "../Config/ConfigService.h"
#include "../IOService/IOService.h"
#include "../List/ListItemService.h"
#include "../List/ListService.h"
#include "../Themes/ThemeService.h"

class RemoveUseCase
{
  public:
    RemoveUseCase(IOService& ioService,
                  Command& command,
                  ListItemService& listItemService,
                  ListService& listService,
                  ConfigService& configService,
                  ThemeService& themeService);
    void execute();

  private:
    IOService& ioService;
    Command& command;
    ListItemService& listItemService;
    ListService& listService;
    ConfigService& configService;
    ThemeService& themeService;
};
