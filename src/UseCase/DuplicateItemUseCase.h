#pragma once

#include "../Command/Command.h"
#include "../Config/ConfigService.h"
#include "../IOService/IOService.h"
#include "../List/ListItemService.h"
#include "../List/ListService.h"
#include "../Themes/ThemeService.h"

class DuplicateItemUseCase
{
  public:
    DuplicateItemUseCase(IOService& ioService,
                         ListService& listService,
                         ListItemService& listItemService,
                         ConfigService& configService,
                         ThemeService& themeService);
    void execute(Command& command, ListName& currentList);

  private:
    IOService& ioService;
    ListService& listService;
    ListItemService& listItemService;
    ConfigService& configService;
    ThemeService& themeService;
};
