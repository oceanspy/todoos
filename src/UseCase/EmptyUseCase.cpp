#include "EmptyUseCase.h"
#include "../Helpers/StringHelpers.h"
#include "../Helpers/TerminalStyle.h"
#include "../List/ListName.h"

EmptyUseCase::EmptyUseCase(IOService& ioService,
                           Command& command,
                           ListItemService& listItemService,
                           ListService& listService,
                           ConfigService& configService)
  : ioService(ioService)
  , command(command)
  , listItemService(listItemService)
  , listService(listService)
  , configService(configService)
{
}

void
EmptyUseCase::execute()
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
    } else {
        ioService.br();
        ioService.message("Action cancelled.");
        ioService.br();
    }
}
