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
ResetUseCase::execute(Command& command)
{
    std::string answer = ioService.ask("Are you sure you want to reset item(s) to brand new and not mark them "
                                       "as to-do? (y/n) ");

    ListName listName =
        listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
    if (answer == "y" || answer == "yes") {
        StatusAction status(ioService, listItemService);
        status.executeReset(command, listName);

        ShowAction show(ioService, listService, listItemService, themeService);

        std::vector<ListItemEntity> listItems = listItemService.get(listName);
        try {
            show.execute(listItems, listName);
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
