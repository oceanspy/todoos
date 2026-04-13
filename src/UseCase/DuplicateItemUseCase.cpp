#include "DuplicateItemUseCase.h"
#include "../Actions/MoveAction/DuplicateItemAction.h"
#include "../Actions/ShowAction/ShowAction.h"
#include "../List/ListItems/ListItemEntity.h"
#include "../List/ListName.h"

DuplicateItemUseCase::DuplicateItemUseCase(IOService& ioService,
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
DuplicateItemUseCase::execute(Command& command)
{
    ListName listName =
        listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());

    DuplicateItemAction action(ioService, listService, listItemService);
    std::string newListNameStr = action.execute(command, listName);

    if (!newListNameStr.empty()) {
        ListName newListName = listService.createListName(newListNameStr, configService.getUsedListVariantStr());
        ShowAction show(ioService, listService, listItemService, themeService);

        std::vector<ListItemEntity> listItems = listItemService.get(newListName);
        try {
            show.print(listItems, newListName);
        } catch (std::exception& e) {
            ioService.br();
            ioService.error(e.what());
            ioService.br();
        }
    }
}
