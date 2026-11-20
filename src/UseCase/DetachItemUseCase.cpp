#include "DetachItemUseCase.h"
#include "../Actions/ParentAction/DetachItemAction.h"
#include "../Actions/ShowAction/ShowAction.h"
#include "../List/ListItems/ListItemEntity.h"
#include "../List/ListName.h"

DetachItemUseCase::DetachItemUseCase(IOService& ioService,
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
DetachItemUseCase::execute(Command& command, ListName& currentList)
{
    DetachItemAction detach(ioService, listItemService);
    detach.execute(command, currentList);

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
