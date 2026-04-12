#include "ListItemActionsUseCase.h"
#include "../Actions/ListItemAction/AddItemAction.h"
#include "../Actions/ListItemAction/AppendItemAction.h"
#include "../Actions/ListItemAction/DeadlineItemAction.h"
#include "../Actions/ListItemAction/EditItemAction.h"
#include "../Actions/ListItemAction/PrependItemAction.h"
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

    if (CommandService::isCommand(command, "add")) {
        AddItemAction itemAction(ioService, commandService, listItemService);
        itemAction.execute(command, listName);
    } else if (CommandService::isCommand(command, "edit")) {
        EditItemAction itemAction(ioService, commandService, listItemService);
        itemAction.execute(command, listName);
    } else if (CommandService::isCommand(command, "append")) {
        AppendItemAction itemAction(ioService, commandService, listItemService);
        itemAction.execute(command, listName);
    } else if (CommandService::isCommand(command, "prepend")) {
        PrependItemAction itemAction(ioService, commandService, listItemService);
        itemAction.execute(command, listName);
    } else if (CommandService::isCommand(command, "deadline")) {
        DeadlineItemAction itemAction(ioService, commandService, listItemService);
        itemAction.execute(command, listName);
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
