#include "StatsUseCase.h"
#include "../Actions/StatsAction/StatsAction.h"
#include "../List/ListName.h"

StatsUseCase::StatsUseCase(IOService& ioService,
                           Command& command,
                           ConfigService& configService,
                           ListItemService& listItemService,
                           ThemeService& themeService,
                           ListService& listService)
  : ioService(ioService)
  , command(command)
  , configService(configService)
  , listItemService(listItemService)
  , themeService(themeService)
  , listService(listService)
{
}

void
StatsUseCase::execute()
{
    ListName listName =
        listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
    StatsAction stats(ioService, configService, command, listItemService, themeService, listName);
    stats.print();
}
