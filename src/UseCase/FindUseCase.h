#pragma once

#include "../Command/Command.h"
#include "../Config/ConfigService.h"
#include "../IOService/IOService.h"
#include "../List/ListItemService.h"
#include "../List/ListService.h"
#include "../Themes/ThemeService.h"

class FindUseCase
{
  public:
    FindUseCase(IOService& ioService,
                Command& command,
                ConfigService& configService,
                ListService& listService,
                ListItemService& listItemService,
                ThemeService& themeService);
    void execute();

  private:
    IOService& ioService;
    Command& command;
    ConfigService& configService;
    ListService& listService;
    ListItemService& listItemService;
    ThemeService& themeService;
};
