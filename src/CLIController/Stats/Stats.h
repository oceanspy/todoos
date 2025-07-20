#ifndef TODOOS_STATS_H
#define TODOOS_STATS_H

#include "../../CLIThemes/CLIThemeService.h"
#include "../../Command/Command.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"

class Stats
{
  public:
    Stats(IOService& ioService,
          ConfigService& configService,
          Command& command,
          ListItemService& listItemService,
          CLIThemeService& cliThemeService);
    void print();

  private:
    IOService& ioService;
    ConfigService& configService;
    Command& command;
    ListItemService& listItemService;
    CLIThemeService& cliThemeService;
};

#endif // TODOOS_STATS_H
