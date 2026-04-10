#include "ListItemActionsUseCase.h"
#include "../Actions/ListItemActions/ListItemActions.h"
#include "../Actions/Show/Show.h"
#include "../Entities/ListItemEntity.h"
#include "../List/ListName.h"

ListItemActionsUseCase::ListItemActionsUseCase(IOService& ioService,
                                               Command& command,
                                               CommandService& commandService,
                                               ListItemService& listItemService,
                                               ListService& listService,
                                               ConfigService& configService,
                                               CLIThemeService& cliThemeService)
  : ioService(ioService)
  , command(command)
  , commandService(commandService)
  , listItemService(listItemService)
  , listService(listService)
  , configService(configService)
  , cliThemeService(cliThemeService)
{
}

void
ListItemActionsUseCase::execute()
{
    ListName listName =
        listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
    ListItemActions listItemActions(ioService, command, commandService, listItemService);
    listItemActions.make(listName);

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
