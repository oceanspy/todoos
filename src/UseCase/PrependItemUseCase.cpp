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
PrependItemUseCase::execute(Command& command, ListName& currentList)
{
    PrependItemAction itemAction(ioService, commandService, listItemService);
    itemAction.execute(command, currentList);

    ShowAction show(ioService, listService, listItemService, themeService);

    std::vector<ListItemEntity> listItems = listItemService.get(currentList);
    try {
        show.execute(listItems, currentList);
    } catch (std::exception& e) {
        ioService.br();
        ioService.error(e.what());
        ioService.br();
    }
}
