#pragma once

#include "../Command/Command.h"
#include "../Command/CommandService.h"
#include "../Config/ConfigService.h"
#include "../IOService/IOService.h"
#include "../List/ListItemService.h"
#include "../List/ListService.h"
#include "../Themes/ThemeService.h"

class MoveUseCase
{
  public:
    MoveUseCase(IOService& ioService,
                Command& command,
                CommandService& commandService,
                ListService& listService,
                ListItemService& listItemService,
                ConfigService& configService,
                ThemeService& themeService);
    void execute();

  private:
    IOService& ioService;
    Command& command;
    CommandService& commandService;
    ListService& listService;
    ListItemService& listItemService;
    ConfigService& configService;
    ThemeService& themeService;
};
