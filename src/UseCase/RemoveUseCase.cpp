#include "RemoveUseCase.h"
#include "../Actions/Remove/Remove.h"
#include "../Actions/Show/Show.h"
#include "../Entities/ListItemEntity.h"
#include "../List/ListName.h"

RemoveUseCase::RemoveUseCase(IOService& ioService,
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
RemoveUseCase::execute()
{
    ListName listName =
        listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
    Remove remove(ioService, command, listItemService);
    remove.remove(listName);

    Show show(ioService, listService, listItemService, cliThemeService);

    std::vector<ListItemEntity> listItems = listItemService.get(listName);
    try {
        show.print(listItems, listName);
    } catch (std::exception& e) {
        ioService.br();
        ioService.error(e.what());
        ioService.br();
    }
}
