#include "MoveUseCase.h"
#include "../Actions/Move/Move.h"
#include "../Actions/Show/Show.h"
#include "../List/ListItems/ListItemEntity.h"
#include "../List/ListName.h"

MoveUseCase::MoveUseCase(IOService& ioService,
                         Command& command,
                         CommandService& commandService,
                         ListService& listService,
                         ListItemService& listItemService,
                         ConfigService& configService,
                         CLIThemeService& cliThemeService)
  : ioService(ioService)
  , command(command)
  , commandService(commandService)
  , listService(listService)
  , listItemService(listItemService)
  , configService(configService)
  , cliThemeService(cliThemeService)
{
}

void
MoveUseCase::execute()
{
    Move move(ioService, command, commandService, listService, listItemService);

    ListName listName =
        listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
    std::string newListNameStr = move.make(listName);
    if (!newListNameStr.empty()) {
        ListName newListName = listService.createListName(newListNameStr, configService.getUsedListVariantStr());
        Show show(ioService, listService, listItemService, cliThemeService);

        std::vector<ListItemEntity> listItems = listItemService.get(newListName);
        try {
            show.print(listItems, newListName);
        } catch (std::exception& e) {
            ioService.br();
            ioService.error(e.what());
            ioService.br();
        }
    }
}
