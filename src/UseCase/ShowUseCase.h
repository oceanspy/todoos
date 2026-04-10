#pragma once

#include <vector>
#include "../CLIThemes/CLIThemeService.h"
#include "../Command/Command.h"
#include "../Command/CommandService.h"
#include "../Config/ConfigService.h"
#include "../Entities/ListItemEntity.h"
#include "../Help/Help.h"
#include "../IOService/IOService.h"
#include "../List/ListItemService.h"
#include "../List/ListService.h"

class ShowUseCase
{
  public:
    ShowUseCase(IOService& ioService,
                Help& help,
                CommandService& commandService,
                Command& command,
                ConfigService& configService,
                ListService& listService,
                ListItemService& listItemService,
                CLIThemeService& cliThemeService);
    void execute();

  private:
    IOService& ioService;
    Help& help;
    CommandService& commandService;
    Command& command;
    ConfigService& configService;
    ListService& listService;
    ListItemService& listItemService;
    CLIThemeService& cliThemeService;
    void filterListItemsWithOptions(std::vector<ListItemEntity>* listItems);
};
