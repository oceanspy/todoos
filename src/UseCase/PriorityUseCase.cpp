#include "PriorityUseCase.h"
#include "../Actions/Priority/Priority.h"
#include "../Actions/Show/Show.h"
#include "../List/ListItems/ListItemEntity.h"
#include "../List/ListName.h"

PriorityUseCase::PriorityUseCase(IOService& ioService,
                                 Command& command,
                                 ListItemService& listItemService,
                                 ListService& listService,
                                 ConfigService& configService,
                                 CLIThemeService& cliThemeService,
                                 std::string action)
  : ioService(ioService)
  , command(command)
  , listItemService(listItemService)
  , listService(listService)
  , configService(configService)
  , cliThemeService(cliThemeService)
  , action(std::move(action))
{
}

void
PriorityUseCase::execute()
{
    ListName listName =
        listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
    Priority priority(ioService, command, listItemService);

    if (action == "set") {
        priority.set(listName);
    } else if (action == "increase") {
        priority.increase(listName);
    } else if (action == "decrease") {
        priority.decrease(listName);
    }

    Show show(ioService, listService, listItemService, cliThemeService);

    std::vector<ListItemEntity> listItems = listItemService.get(listName);
    try {
        show.print(listItems, listName);
    } catch (std::exception& e) {
        ioService.br();
        ioService.error(e.what());
        ioService.br();
    }
}
