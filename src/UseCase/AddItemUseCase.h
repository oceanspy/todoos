#pragma once

#include "../Command/Command.h"
#include "../Command/CommandService.h"
#include "../Config/ConfigService.h"
#include "../IOService/IOService.h"
#include "../List/ListItemService.h"
#include "../List/ListService.h"
#include "../Themes/ThemeService.h"
#include "DescribeItemUseCase.h"

class AddItemUseCase
{
  public:
    AddItemUseCase(IOService& ioService,
                   CommandService& commandService,
                   ListItemService& listItemService,
                   ListService& listService,
                   ConfigService& configService,
                   ThemeService& themeService,
                   DescribeItemUseCase& describeItemUseCase);
    void execute(Command& command, ListName& currentList);

  private:
    IOService& ioService;
    CommandService& commandService;
    ListItemService& listItemService;
    ListService& listService;
    ConfigService& configService;
    ThemeService& themeService;
    DescribeItemUseCase& describeItemUseCase;
};
