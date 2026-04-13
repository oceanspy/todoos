#include "StatsUseCase.h"
#include "../Actions/StatsAction/StatsAction.h"
#include "../List/ListName.h"

StatsUseCase::StatsUseCase(IOService& ioService,
                           ConfigService& configService,
                           ListItemService& listItemService,
                           ThemeService& themeService,
                           ListService& listService)
  : ioService(ioService)
  , configService(configService)
  , listItemService(listItemService)
  , themeService(themeService)
  , listService(listService)
{
}

void
StatsUseCase::execute(Command& command)
{
    ListName listName =
        listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
    StatsAction stats(ioService, configService, listItemService, themeService);
    stats.execute(command, listName);
}
