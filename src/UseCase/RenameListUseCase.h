#pragma once

#include "../Command/Command.h"
#include "../Config/ConfigService.h"
#include "../IOService/IOService.h"
#include "../List/ListService.h"
#include "../Themes/ThemeService.h"

class RenameListUseCase
{
  public:
    RenameListUseCase(IOService& ioService,
                      ListService& listService,
                      ConfigService& configService,
                      ThemeService& themeService);
    void execute(Command& command);

  private:
    IOService& ioService;
    ListService& listService;
    ConfigService& configService;
    ThemeService& themeService;
};
