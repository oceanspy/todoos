#pragma once

#include "../CLIThemes/CLIThemeService.h"
#include "../Command/Command.h"
#include "../Command/CommandService.h"
#include "../Config/ConfigService.h"
#include "../FileStorage/FileStorageService.h"
#include "../IOService/IOService.h"
#include "../List/ListItemService.h"
#include "../List/ListService.h"

class ListUseCase
{
  public:
    ListUseCase(IOService& ioService,
                Command& command,
                CommandService& commandService,
                ListService& listService,
                ListItemService& listItemService,
                FileStorageService& fileStorageService,
                ConfigService& configService,
                CLIThemeService& cliThemeService);
    void execute();

  private:
    IOService& ioService;
    Command& command;
    CommandService& commandService;
    ListService& listService;
    ListItemService& listItemService;
    FileStorageService& fileStorageService;
    ConfigService& configService;
    CLIThemeService& cliThemeService;
};
