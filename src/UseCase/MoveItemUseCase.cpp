#include "MoveItemUseCase.h"
#include "../Actions/MoveAction/MoveItemAction.h"
#include "../Actions/ShowAction/ShowAction.h"
#include "../List/ListItems/ListItemEntity.h"
#include "../List/ListName.h"

MoveItemUseCase::MoveItemUseCase(IOService& ioService,
                                 CommandService& commandService,
                                 ListService& listService,
                                 ListItemService& listItemService,
                                 ConfigService& configService,
                                 ThemeService& themeService)
  : ioService(ioService)
  , commandService(commandService)
  , listService(listService)
  , listItemService(listItemService)
  , configService(configService)
  , themeService(themeService)
{
}

void
MoveItemUseCase::execute(Command& command, ListName& currentList)
{
    MoveItemAction move(ioService, listService, listItemService);

    std::string newListNameStr = move.execute(command, currentList);
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
