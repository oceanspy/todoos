#include "PrependItemUseCase.h"
#include "../Actions/ListItemAction/PrependItemAction.h"
#include "../Actions/ShowAction/ShowAction.h"
#include "../List/ListItems/ListItemEntity.h"
#include "../List/ListName.h"

PrependItemUseCase::PrependItemUseCase(IOService& ioService,
                                       CommandService& commandService,
                                       ListItemService& listItemService,
                                       ListService& listService,
                                       ConfigService& configService,
                                       ThemeService& themeService)
  : ioService(ioService)
  , commandService(commandService)
  , listItemService(listItemService)
  , listService(listService)
  , configService(configService)
  , themeService(themeService)
{
}

void
PrependItemUseCase::execute(Command& command)
{
    ListName listName =
        listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());

    PrependItemAction itemAction(ioService, commandService, listItemService);
    itemAction.execute(command, listName);

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
