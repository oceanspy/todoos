#include "ResetUseCase.h"
#include "../Actions/Show/Show.h"
#include "../Actions/Status/Status.h"
#include "../Entities/ListItemEntity.h"
#include "../List/ListName.h"

ResetUseCase::ResetUseCase(IOService& ioService,
                            Command& command,
                            ListItemService& listItemService,
                            ListService& listService,
                            ConfigService& configService,
                            CLIThemeService& cliThemeService)
  : ioService(ioService)
  , command(command)
  , listItemService(listItemService)
  , listService(listService)
  , configService(configService)
  , cliThemeService(cliThemeService)
{
}

void
ResetUseCase::execute()
{
    std::string answer = ioService.ask("Are you sure you want to reset item(s) to brand new and not mark them "
                                       "as to-do? (y/n) ");

    ListName listName =
        listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
    if (answer == "y" || answer == "yes") {
        Status status(ioService, command, listItemService);
        status.reset(listName);

        Show show(ioService, listService, listItemService, cliThemeService);

        std::vector<ListItemEntity> listItems = listItemService.get(listName);
        try {
            show.print(listItems, listName);
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
