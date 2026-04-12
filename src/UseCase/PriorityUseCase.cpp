#include "PriorityUseCase.h"
#include "../Actions/PriorityAction/PriorityAction.h"
#include "../Actions/ShowAction/ShowAction.h"
#include "../List/ListItems/ListItemEntity.h"
#include "../List/ListName.h"

PriorityUseCase::PriorityUseCase(IOService& ioService,
                                 Command& command,
                                 ListItemService& listItemService,
                                 ListService& listService,
                                 ConfigService& configService,
                                 ThemeService& themeService,
                                 std::string action)
  : ioService(ioService)
  , command(command)
  , listItemService(listItemService)
  , listService(listService)
  , configService(configService)
  , themeService(themeService)
  , action(std::move(action))
{
}

void
PriorityUseCase::execute()
{
    ListName listName =
        listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
    PriorityAction priority(ioService, command, listItemService);

    if (action == "set") {
        priority.set(listName);
    } else if (action == "increase") {
        priority.increase(listName);
    } else if (action == "decrease") {
        priority.decrease(listName);
    }

    ShowAction show(ioService, listService, listItemService, themeService);

    std::vector<ListItemEntity> listItems = listItemService.get(listName);
    try {
        show.print(listItems, listName);
    } catch (std::exception& e) {
        ioService.br();
        ioService.error(e.what());
        ioService.br();
    }
}
