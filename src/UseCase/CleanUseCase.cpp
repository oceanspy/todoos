#include "CleanUseCase.h"
#include "../Actions/Show/Show.h"
#include "../Entities/ListItemEntity.h"
#include "../Helpers/BashStyle.h"
#include "../Helpers/StringHelpers.h"
#include "../List/ListName.h"

CleanUseCase::CleanUseCase(IOService& ioService,
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
CleanUseCase::execute()
{
    ioService.br();
    std::string message = "Are you sure you want to archive all completed & cancelled from ";
    message += StringHelpers::colorize(configService.getUsedListNameStr(), RED);
    message += " list? (yes/no) ";
    std::string answer = ioService.ask(message);

    if (answer == "yes") {
        ListName listName =
            listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
        listItemService.archiveFinishedItems(listName);
        ioService.br();
        ioService.success("List cleaned.");
        ioService.br();

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
