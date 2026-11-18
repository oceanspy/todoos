#include "ReassignItemUseCase.h"
#include "../Actions/ListItemAction/ReassignItemAction.h"
#include "../Actions/ShowAction/ShowAction.h"
#include "../List/ListItems/ListItemEntity.h"
#include "../List/ListName.h"

ReassignItemUseCase::ReassignItemUseCase(IOService& ioService,
                                         ListItemService& listItemService,
                                         ListService& listService,
                                         ThemeService& themeService)
  : ioService(ioService)
  , listItemService(listItemService)
  , listService(listService)
  , themeService(themeService)
{
}

void
ReassignItemUseCase::execute(Command& command, ListName& currentList)
{
    ReassignItemAction reassign(ioService, listItemService);
    reassign.execute(command, currentList);

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
