#include "StatusUseCase.h"
#include "../Actions/Show/Show.h"
#include "../Actions/Status/Status.h"
#include "../List/ListItems/ListItemEntity.h"
#include "../List/ListName.h"

StatusUseCase::StatusUseCase(IOService& ioService,
                             Command& command,
                             ListItemService& listItemService,
                             ListService& listService,
                             ConfigService& configService,
                             CLIThemeService& cliThemeService,
                             int statusNumber)
  : ioService(ioService)
  , command(command)
  , listItemService(listItemService)
  , listService(listService)
  , configService(configService)
  , cliThemeService(cliThemeService)
  , statusNumber(statusNumber)
{
}

void
StatusUseCase::execute()
{
    ListName listName =
        listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
    Status status(ioService, command, listItemService);
    if (statusNumber == -1) {
        status.set(listName);
    } else {
        status.markAs(listName, statusNumber);
    }

    Show show(ioService, listService, listItemService, cliThemeService);

    std::vector<ListItemEntity> listItems = listItemService.get(listName);
    try {
        show.print(listItems, listName);
    } catch (std::exception& e) {
        ioService.br();
        ioService.error(e.what());
        ioService.br();
    }
}
