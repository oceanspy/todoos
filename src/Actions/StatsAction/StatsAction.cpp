#include "StatsAction.h"
#include "../../Themes/Theme.h"

StatsAction::StatsAction(IOService& ioService,
                         ConfigService& configService,
                         Command& command,
                         ListItemService& listItemService,
                         ThemeService& themeService,
                         ListName& listName)
  : ioService(ioService)
  , configService(configService)
  , command(command)
  , listItemService(listItemService)
  , themeService(themeService)
  , listName(listName)
{
}

void
StatsAction::print()
{
    auto theme = themeService.getTheme();
    theme->printATitle("Statistics", "List: " + listName.getName());
    theme->printStats(listName);
}
