#ifndef TODOOS_STATS_H
#define TODOOS_STATS_H

#include "../../Command/Command.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"
#include "../../List/ListName.h"
#include "../../Themes/ThemeService.h"

class Stats
{
  public:
    Stats(IOService& ioService,
          ConfigService& configService,
          Command& command,
          ListItemService& listItemService,
          ThemeService& themeService,
          ListName& listName);
    void print();

  private:
    IOService& ioService;
    ConfigService& configService;
    Command& command;
    ListItemService& listItemService;
    ThemeService& themeService;
    ListName& listName;
};

#endif // TODOOS_STATS_H
