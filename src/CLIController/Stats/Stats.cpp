#include "Stats.h"
#include "../../CLIThemes/ThemeAbstract.h"

Stats::Stats(IOService &ioService, ConfigService& configService, Command &command, ListItemService &listItemService, CLIThemeService &cliThemeService)
        : ioService(ioService), configService(configService), command(command), listItemService(listItemService), cliThemeService(cliThemeService) { }

void Stats::print()
{
    ThemeAbstract* theme = cliThemeService.getTheme();
    theme->printATitle("Statistics", "List: " + configService.getCurrentList());
    theme->printStats();
}