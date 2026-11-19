#pragma once

#include "../Command/Command.h"
#include "../IOService/IOService.h"
#include "../List/ListItemService.h"
#include "../List/ListService.h"
#include "../Themes/ThemeService.h"

class AttachItemUseCase
{
  public:
    AttachItemUseCase(IOService& ioService,
                    ListItemService& listItemService,
                    ListService& listService,
                    ThemeService& themeService);
    void execute(Command& command, ListName& currentList);

  private:
    IOService& ioService;
    ListItemService& listItemService;
    ListService& listService;
    ThemeService& themeService;
};
