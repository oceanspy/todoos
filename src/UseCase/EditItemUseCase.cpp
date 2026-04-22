#include "EditItemUseCase.h"
#include "../Actions/ListItemAction/EditItemAction.h"
#include "../Actions/ShowAction/ShowAction.h"
#include "../List/ListItems/ListItemEntity.h"
#include "../List/ListName.h"

EditItemUseCase::EditItemUseCase(IOService& ioService,
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
EditItemUseCase::execute(Command& command, ListName& currentList)
{
    EditItemAction itemAction(ioService, commandService, listItemService);
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
