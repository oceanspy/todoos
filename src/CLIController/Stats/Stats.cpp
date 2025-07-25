#include "Stats.h"
#include "../../CLIThemes/ThemeAbstract.h"

Stats::Stats(IOService& ioService,
             ConfigService& configService,
             Command& command,
             ListItemService& listItemService,
             CLIThemeService& cliThemeService,
             std::string& listName,
             std::string& listVariant)
  : ioService(ioService)
  , configService(configService)
  , command(command)
  , listItemService(listItemService)
  , cliThemeService(cliThemeService)
  , listName(listName)
  , listVariant(listVariant)
{
}

void
Stats::print()
{
    ThemeAbstract* theme = cliThemeService.getTheme();
    theme->printATitle("Statistics", "List: " + configService.getCurrentList());
    theme->printStats(listName, listVariant);
}
