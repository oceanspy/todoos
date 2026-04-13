#include "StatusUseCase.h"
#include "../Actions/ShowAction/ShowAction.h"
#include "../Actions/StatusAction/StatusAction.h"
#include "../List/ListItems/ListItemEntity.h"
#include "../List/ListName.h"

StatusUseCase::StatusUseCase(IOService& ioService,
                             ListItemService& listItemService,
                             ListService& listService,
                             ConfigService& configService,
                             ThemeService& themeService)
  : ioService(ioService)
  , listItemService(listItemService)
  , listService(listService)
  , configService(configService)
  , themeService(themeService)
{
}

void
StatusUseCase::execute(Command& command, int statusNumber)
{
    ListName listName =
        listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
    StatusAction status(ioService, listItemService);
    status.execute(command, listName, statusNumber);

    ShowAction show(ioService, listService, listItemService, themeService);
    std::vector<ListItemEntity> listItems = listItemService.get(listName);
    try {
        show.execute(listItems, listName);
    } catch (std::exception& e) {
        ioService.br();
        ioService.error(e.what());
        ioService.br();
    }
}
