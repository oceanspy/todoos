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
PriorityUseCase::execute(Command& command, ListName& currentList, const std::string& action)
{
    PriorityAction priority(ioService, listItemService);
    priority.execute(command, currentList, action);

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
