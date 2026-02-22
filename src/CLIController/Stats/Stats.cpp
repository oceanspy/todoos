#include "Stats.h"
#include "../../CLIThemes/ThemeAbstract.h"

Stats::Stats(IOService& ioService,
             ConfigService& configService,
             Command& command,
             ListItemService& listItemService,
             CLIThemeService& cliThemeService,
             ListName& listName)
  : ioService(ioService)
  , configService(configService)
  , command(command)
  , listItemService(listItemService)
  , cliThemeService(cliThemeService)
  , listName(listName)
{
}

void
Stats::print()
{
    ThemeAbstract* theme = cliThemeService.getTheme();
    theme->printATitle("Statistics", "List: " + listName.getName());
    theme->printStats(listName);
}
