#include "ResetUseCase.h"
#include "../Actions/ShowAction/ShowAction.h"
#include "../Actions/StatusAction/StatusAction.h"
#include "../List/ListItems/ListItemEntity.h"
#include "../List/ListName.h"

ResetUseCase::ResetUseCase(IOService& ioService,
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
ResetUseCase::execute(Command& command, ListName& currentList)
{
    std::string answer =
        ioService.ask("Are you sure you want to reset item(s) to queued and clear date/deadline? (y/n) ");

    if (answer == "y" || answer == "yes") {
        StatusAction status(ioService, listItemService);
        status.executeReset(command, currentList);

        ShowAction show(ioService, listService, listItemService, themeService);

        std::vector<ListItemEntity> listItems = listItemService.get(currentList);
        try {
            show.execute(listItems, currentList);
        } catch (std::exception& e) {
            ioService.br();
            ioService.error(e.what());
            ioService.br();
        }
    } else {
        ioService.br();
        ioService.message("Action cancelled.");
        ioService.br();
    }
}
