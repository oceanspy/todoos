#pragma once

#include "../Command/Command.h"
#include "../Config/ConfigService.h"
#include "../FileStorage/FileStorageService.h"
#include "../IOService/IOService.h"
#include "../List/ListService.h"
#include "../Themes/ThemeService.h"

class AddListUseCase
{
  public:
    AddListUseCase(IOService& ioService,
                   ListService& listService,
                   FileStorageService& fileStorageService,
                   ConfigService& configService,
                   ThemeService& themeService);
    void execute(Command& command);

  private:
    IOService& ioService;
    ListService& listService;
    FileStorageService& fileStorageService;
    ConfigService& configService;
    ThemeService& themeService;
};
