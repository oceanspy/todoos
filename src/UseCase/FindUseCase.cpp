#include "FindUseCase.h"
#include "../Actions/Show/Show.h"
#include "../List/ListItems/ListItemEntity.h"
#include "../List/ListName.h"

FindUseCase::FindUseCase(IOService& ioService,
                         Command& command,
                         ConfigService& configService,
                         ListService& listService,
                         ListItemService& listItemService,
                         CLIThemeService& cliThemeService)
  : ioService(ioService)
  , command(command)
  , configService(configService)
  , listService(listService)
  , listItemService(listItemService)
  , cliThemeService(cliThemeService)
{
}

void
FindUseCase::execute()
{
    ListName listName =
        listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
    ListName listNameArchive = ListName::createVariant(listName, "archive");
    ListName listNameDelete = ListName::createVariant(listName, "delete");
    Show show(ioService, listService, listItemService, cliThemeService);

    std::string stringSearch;
    for (const std::string& argument : command.getArguments()) {
        stringSearch += argument + " ";
    }

    try {
        std::vector<ListItemEntity> listItems;
        if (command.hasOption("archive")) {
            listItems = listItemService.search(listNameArchive, command.getArguments());

            ioService.br();
            ioService.info("Searching for " + stringSearch + "in archived items...");
            ioService.br();

            show.print(listItems, listName);
        } else if (command.hasOption("delete")) {
            listItems = listItemService.search(listNameDelete, command.getArguments());

            ioService.br();
            ioService.info("Searching for " + stringSearch + "in deleted items...");
            ioService.br();

            try {
                show.print(listItems, listName);
            } catch (std::exception& e) {
                ioService.br();
                ioService.error(e.what());
                ioService.br();
            }
        } else {
            listItems = listItemService.search(listName, command.getArguments());
            ioService.br();
            ioService.info("Searching for " + stringSearch + "...");
            ioService.br();
            try {
                show.print(listItems, listName);
            } catch (std::exception& e) {
                ioService.br();
                ioService.error(e.what());
                ioService.br();
            }
        }
    } catch (std::exception& e) {
        ioService.br();
        ioService.info("Searching for " + stringSearch + "...");
        ioService.br();
        ioService.error("No item found.");
    }
}
