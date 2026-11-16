#include "DuplicateItemUseCase.h"
#include "../Actions/MoveAction/DuplicateItemAction.h"
#include "../Actions/ShowAction/ShowAction.h"
#include "../List/ListItems/ListItemEntity.h"
#include "../List/ListName.h"

DuplicateItemUseCase::DuplicateItemUseCase(IOService& ioService,
                                           ListService& listService,
                                           ListItemService& listItemService,
                                           ConfigService& configService,
                                           ThemeService& themeService,
                                           DescriptionService& descriptionService)
  : ioService(ioService)
  , listService(listService)
  , listItemService(listItemService)
  , configService(configService)
  , themeService(themeService)
  , descriptionService(descriptionService)
{
}

void
DuplicateItemUseCase::execute(Command& command, ListName& currentList)
{
    DuplicateItemAction action(ioService, listService, listItemService, descriptionService);
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
