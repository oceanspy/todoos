#include "RestoreUseCase.h"
#include "../Actions/RemoveAction/RemoveAction.h"
#include "../Actions/ShowAction/ShowAction.h"
#include "../List/ListItems/ListItemEntity.h"
#include "../List/ListName.h"

RestoreUseCase::RestoreUseCase(IOService& ioService,
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
RestoreUseCase::execute(Command& command)
{
    ListName listName =
        listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
    RemoveAction remove(ioService, listItemService);
    remove.execute(command, listName, "restore");

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
