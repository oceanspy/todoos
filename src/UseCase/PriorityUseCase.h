#pragma once

#include "../Command/Command.h"
#include "../Config/ConfigService.h"
#include "../IOService/IOService.h"
#include "../List/ListItemService.h"
#include "../List/ListService.h"
#include "../Themes/ThemeService.h"
#include <string>

class PriorityUseCase
{
  public:
    PriorityUseCase(IOService& ioService,
                    ListItemService& listItemService,
                    ListService& listService,
                    ConfigService& configService,
                    ThemeService& themeService,
                    std::string action);
    void execute(Command& command);

  private:
    IOService& ioService;
    ListItemService& listItemService;
    ListService& listService;
    ConfigService& configService;
    ThemeService& themeService;
    std::string action;
};
