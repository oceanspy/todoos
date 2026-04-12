#include "RestoreUseCase.h"
#include "../Actions/RemoveAction/RemoveAction.h"
#include "../Actions/ShowAction/ShowAction.h"
#include "../List/ListItems/ListItemEntity.h"
#include "../List/ListName.h"

RestoreUseCase::RestoreUseCase(IOService& ioService,
                               Command& command,
                               ListItemService& listItemService,
                               ListService& listService,
                               ConfigService& configService,
                               ThemeService& themeService)
  : ioService(ioService)
  , command(command)
  , listItemService(listItemService)
  , listService(listService)
  , configService(configService)
  , themeService(themeService)
{
}

void
RestoreUseCase::execute()
{
    ListName listName =
        listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
    RemoveAction remove(ioService, command, listItemService);
    remove.restore(listName);

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
