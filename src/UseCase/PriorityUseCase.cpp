#include "PriorityUseCase.h"
#include "../Actions/PriorityAction/PriorityAction.h"
#include "../Actions/ShowAction/ShowAction.h"
#include "../List/ListItems/ListItemEntity.h"
#include "../List/ListName.h"

PriorityUseCase::PriorityUseCase(IOService& ioService,
                                 ListItemService& listItemService,
                                 ListService& listService,
                                 ConfigService& configService,
                                 ThemeService& themeService)
  : ioService(ioService)
  , listItemService(listItemService)
  , listService(listService)
  , configService(configService)
  , themeService(themeService)
{
}

void
PriorityUseCase::execute(Command& command, const std::string& action)
{
    ListName listName =
        listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
    PriorityAction priority(ioService, listItemService);
    priority.execute(command, listName, action);

    ShowAction show(ioService, listService, listItemService, themeService);
    std::vector<ListItemEntity> listItems = listItemService.get(listName);
    try {
        show.execute(listItems, listName);
    } catch (std::exception& e) {
        ioService.br();
        ioService.error(e.what());
        ioService.br();
    }
}
