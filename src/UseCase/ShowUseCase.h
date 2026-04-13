#pragma once

#include "../Command/Command.h"
#include "../Command/CommandService.h"
#include "../Config/ConfigService.h"
#include "../Help/HelpPrinter.h"
#include "../IOService/IOService.h"
#include "../List/ListItemService.h"
#include "../List/ListItems/ListItemEntity.h"
#include "../List/ListService.h"
#include "../Themes/ThemeService.h"
#include <vector>

class ShowUseCase
{
  public:
    ShowUseCase(IOService& ioService,
                HelpPrinter& help,
                CommandService& commandService,
                ConfigService& configService,
                ListService& listService,
                ListItemService& listItemService,
                ThemeService& themeService);
    void execute(Command& command);

  private:
    IOService& ioService;
    HelpPrinter& help;
    CommandService& commandService;
    ConfigService& configService;
    ListService& listService;
    ListItemService& listItemService;
    ThemeService& themeService;
    void filterListItemsWithOptions(Command& command, std::vector<ListItemEntity>& listItems);
};
