#include "AttachItemUseCase.h"
#include "../Actions/ParentAction/AttachItemAction.h"
#include "../Actions/ShowAction/ShowAction.h"
#include "../List/ListItems/ListItemEntity.h"
#include "../List/ListName.h"

AttachItemUseCase::AttachItemUseCase(IOService& ioService,
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
AttachItemUseCase::execute(Command& command, ListName& currentList)
{
    AttachItemAction attach(ioService, listItemService);
    attach.execute(command, currentList);

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
