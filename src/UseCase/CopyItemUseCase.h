#pragma once

#include "../Command/Command.h"
#include "../Config/ConfigService.h"
#include "../Describe/DescriptionService.h"
#include "../IOService/IOService.h"
#include "../List/ListItemService.h"
#include "../List/ListService.h"
#include "../Themes/ThemeService.h"

class CopyItemUseCase
{
  public:
    CopyItemUseCase(IOService& ioService,
                    ListService& listService,
                    ListItemService& listItemService,
                    ConfigService& configService,
                    ThemeService& themeService,
                    DescriptionService& descriptionService);
    void execute(Command& command, ListName& currentList);

  private:
    IOService& ioService;
    ListService& listService;
    ListItemService& listItemService;
    ConfigService& configService;
    ThemeService& themeService;
    DescriptionService& descriptionService;
};
