#include "StatsAction.h"
#include "../../Themes/Theme.h"

StatsAction::StatsAction(IOService& ioService,
                         ConfigService& configService,
                         ListItemService& listItemService,
                         ThemeService& themeService)
  : ioService(ioService)
  , configService(configService)
  , listItemService(listItemService)
  , themeService(themeService)
{
}

void
StatsAction::execute(Command& command, ListName& listName)
{
    auto theme = themeService.getTheme();
    theme->printATitle("Statistics", "List: " + listName.getName());
    theme->printStats(listName);
}
