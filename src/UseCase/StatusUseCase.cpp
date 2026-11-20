#include "StatusUseCase.h"
#include "../Actions/ShowAction/ShowAction.h"
#include "../Actions/StatusAction/EditStatusAction.h"
#include "../Actions/StatusAction/SetStatusAction.h"
#include "../List/ListItems/ListItemEntity.h"
#include "../List/ListName.h"

StatusUseCase::StatusUseCase(IOService& ioService,
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
StatusUseCase::editStatus(Command& command, ListName& currentList, int statusNumber)
{
    EditStatusAction status(ioService, listItemService);
    status.execute(command, currentList, statusNumber);

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
void
StatusUseCase::setStatus(Command& command, ListName& currentList)
{
    SetStatusAction status(ioService, listItemService);
    status.execute(command, currentList);

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
