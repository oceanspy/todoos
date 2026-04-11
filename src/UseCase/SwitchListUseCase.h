#pragma once

#include "../Command/Command.h"
#include "../Command/CommandService.h"
#include "../Config/ConfigService.h"
#include "../FileStorage/FileStorageService.h"
#include "../IOService/IOService.h"
#include "../List/ListItemService.h"
#include "../List/ListService.h"
#include "../Themes/ThemeService.h"

class SwitchListUseCase
{
  public:
    SwitchListUseCase(IOService& ioService,
                      Command& command,
                      CommandService& commandService,
                      ListService& listService,
                      ListItemService& listItemService,
                      FileStorageService& fileStorageService,
                      ConfigService& configService,
                      ThemeService& themeService);
    void execute();

  private:
    IOService& ioService;
    Command& command;
    CommandService& commandService;
    ListService& listService;
    ListItemService& listItemService;
    FileStorageService& fileStorageService;
    ConfigService& configService;
    ThemeService& themeService;
};
