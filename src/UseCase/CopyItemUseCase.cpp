#include "CopyItemUseCase.h"
#include "../Actions/MoveAction/CopyItemAction.h"
#include "../Actions/ShowAction/ShowAction.h"
#include "../List/ListItems/ListItemEntity.h"
#include "../List/ListName.h"

CopyItemUseCase::CopyItemUseCase(IOService& ioService,
                                 ListService& listService,
                                 ListItemService& listItemService,
                                 ConfigService& configService,
                                 ThemeService& themeService)
  : ioService(ioService)
  , listService(listService)
  , listItemService(listItemService)
  , configService(configService)
  , themeService(themeService)
{
}

void
CopyItemUseCase::execute(Command& command, ListName& currentList)
{
    CopyItemAction action(ioService, listService, listItemService);
    std::string newListNameStr = action.execute(command, currentList);

    if (!newListNameStr.empty()) {
        ListName newListName = listService.createListName(newListNameStr, configService.getUsedListVariantStr());
        ShowAction show(ioService, listService, listItemService, themeService);

        std::vector<ListItemEntity> listItems = listItemService.get(newListName);
        try {
            show.execute(listItems, newListName);
        } catch (std::exception& e) {
            ioService.br();
            ioService.error(e.what());
            ioService.br();
        }
    }
}
