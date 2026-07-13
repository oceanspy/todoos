#pragma once

#include "../Command/Command.h"
#include "../Config/ConfigService.h"
#include "../IOService/IOService.h"
#include "../List/ListItemService.h"
#include "../List/ListService.h"
#include "../Themes/ThemeService.h"

class ArchiveUseCase
{
  public:
    ArchiveUseCase(IOService& ioService,
                   ListItemService& listItemService,
                   ListService& listService,
                   ConfigService& configService,
                   ThemeService& themeService);
    void execute(Command& command, ListName& currentList);

  private:
    IOService& ioService;
    ListItemService& listItemService;
    ListService& listService;
    ConfigService& configService;
    ThemeService& themeService;
};
