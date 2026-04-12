#ifndef TODOOS_STATSACTION_H
#define TODOOS_STATSACTION_H

#include "../../Command/Command.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"
#include "../../List/ListName.h"
#include "../../Themes/ThemeService.h"

class StatsAction
{
  public:
    StatsAction(IOService& ioService,
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

#endif // TODOOS_STATSACTION_H
