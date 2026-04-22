#pragma once

#include "../Command/Command.h"
#include "../Command/CommandService.h"
#include "../Config/ConfigService.h"
#include "../IOService/IOService.h"
#include "../List/ListItemService.h"
#include "../List/ListService.h"
#include "../Themes/ThemeService.h"

class DeadlineItemUseCase
{
  public:
    DeadlineItemUseCase(IOService& ioService,
                        CommandService& commandService,
                        ListItemService& listItemService,
                        ListService& listService,
                        ConfigService& configService,
                        ThemeService& themeService);
    void execute(Command& command, ListName& currentList);

  private:
    IOService& ioService;
    CommandService& commandService;
    ListItemService& listItemService;
    ListService& listService;
    ConfigService& configService;
    ThemeService& themeService;
};
