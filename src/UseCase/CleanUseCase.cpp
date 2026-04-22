#include "CleanUseCase.h"
#include "../Actions/ShowAction/ShowAction.h"
#include "../Helpers/StringHelpers.h"
#include "../Helpers/TerminalStyle.h"
#include "../List/ListItems/ListItemEntity.h"
#include "../List/ListName.h"

CleanUseCase::CleanUseCase(IOService& ioService,
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
CleanUseCase::execute(Command& command, ListName& currentList)
{
    ioService.br();
    std::string message = "Are you sure you want to archive all completed & cancelled from ";
    message += StringHelpers::colorize(configService.getUsedListNameStr(), RED);
    message += " list? (yes/no) ";
    std::string answer = ioService.ask(message);

    if (answer == "yes") {
        listItemService.archiveFinishedItems(currentList);
        ioService.br();
        ioService.success("List cleaned.");
        ioService.br();

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
