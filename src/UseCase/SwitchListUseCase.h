#pragma once

#include "../Command/Command.h"
#include "../Command/CommandService.h"
#include "../Config/ConfigService.h"
#include "../IOService/IOService.h"
#include "../List/ListItemService.h"
#include "../List/ListService.h"
#include "../Themes/ThemeService.h"

class SwitchListUseCase
{
  public:
    SwitchListUseCase(IOService& ioService,
                      CommandService& commandService,
                      ListService& listService,
                      ListItemService& listItemService,
                      ConfigService& configService,
                      ThemeService& themeService);
    void execute(Command& command);

  private:
    IOService& ioService;
    CommandService& commandService;
    ListService& listService;
    ListItemService& listItemService;
    ConfigService& configService;
    ThemeService& themeService;
};
