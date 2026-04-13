#pragma once

#include "../Command/Command.h"
#include "../Config/ConfigService.h"
#include "../FileStorage/FileStorageService.h"
#include "../IOService/IOService.h"
#include "../List/ListItemService.h"
#include "../List/ListService.h"
#include "../Themes/ThemeService.h"

class CopyListUseCase
{
  public:
    CopyListUseCase(IOService& ioService,
                    ListService& listService,
                    ListItemService& listItemService,
                    FileStorageService& fileStorageService,
                    ConfigService& configService,
                    ThemeService& themeService);
    void execute(Command& command);

  private:
    IOService& ioService;
    ListService& listService;
    ListItemService& listItemService;
    FileStorageService& fileStorageService;
    ConfigService& configService;
    ThemeService& themeService;
};
