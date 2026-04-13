#include "EmptyUseCase.h"
#include "../Actions/ShowAction/ShowAction.h"
#include "../Helpers/StringHelpers.h"
#include "../Helpers/TerminalStyle.h"
#include "../List/ListName.h"

EmptyUseCase::EmptyUseCase(IOService& ioService,
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
EmptyUseCase::execute(Command& command)
{
    ioService.br();
    std::string message = "Are you sure you want to empty ";
    message += StringHelpers::colorize(configService.getUsedListNameStr(), RED);
    message += " list? (yes/no) ";
    std::string answer = ioService.ask(message);

    if (answer == "yes") {
        ListName listName =
            listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
        listItemService.archiveAll(listName);
        ioService.br();
        ioService.success("List cleared.");
        ioService.br();

        listService.use(configService.getDefaultList());
        ListName defaultListName =
            listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
        ShowAction show(ioService, listService, listItemService, themeService);
        std::vector<ListItemEntity> listItems = listItemService.get(defaultListName);
        try {
            show.print(listItems, defaultListName);
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
