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
                ListItemService& listItemService,
                ThemeService& themeService);
    void execute(Command& command, ListName& listName);

  private:
    IOService& ioService;
    ConfigService& configService;
    ListItemService& listItemService;
    ThemeService& themeService;
};

#endif // TODOOS_STATSACTION_H
