#include "StatusUseCase.h"
#include "../Actions/ShowAction/ShowAction.h"
#include "../Actions/StatusAction/StatusAction.h"
#include "../List/ListItems/ListItemEntity.h"
#include "../List/ListName.h"

StatusUseCase::StatusUseCase(IOService& ioService,
                             Command& command,
                             ListItemService& listItemService,
                             ListService& listService,
                             ConfigService& configService,
                             ThemeService& themeService,
                             int statusNumber)
  : ioService(ioService)
  , command(command)
  , listItemService(listItemService)
  , listService(listService)
  , configService(configService)
  , themeService(themeService)
  , statusNumber(statusNumber)
{
}

void
StatusUseCase::execute()
{
    ListName listName =
        listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
    StatusAction status(ioService, command, listItemService);
    if (statusNumber == -1) {
        status.set(listName);
    } else {
        status.markAs(listName, statusNumber);
    }

    ShowAction show(ioService, listService, listItemService, themeService);

    std::vector<ListItemEntity> listItems = listItemService.get(listName);
    try {
        show.print(listItems, listName);
    } catch (std::exception& e) {
        ioService.br();
        ioService.error(e.what());
        ioService.br();
    }
}
