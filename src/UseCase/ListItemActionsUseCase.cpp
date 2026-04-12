#include "ListItemActionsUseCase.h"
#include "../Actions/ListItemAction/ListItemAction.h"
#include "../Actions/ShowAction/ShowAction.h"
#include "../List/ListItems/ListItemEntity.h"
#include "../List/ListName.h"

ListItemActionsUseCase::ListItemActionsUseCase(IOService& ioService,
                                               Command& command,
                                               CommandService& commandService,
                                               ListItemService& listItemService,
                                               ListService& listService,
                                               ConfigService& configService,
                                               ThemeService& themeService)
  : ioService(ioService)
  , command(command)
  , commandService(commandService)
  , listItemService(listItemService)
  , listService(listService)
  , configService(configService)
  , themeService(themeService)
{
}

void
ListItemActionsUseCase::execute()
{
    ListName listName =
        listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
    ListItemAction listItemActions(ioService, command, commandService, listItemService);
    listItemActions.make(listName);

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
