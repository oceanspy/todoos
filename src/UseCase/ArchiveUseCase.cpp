#include "ArchiveUseCase.h"
#include "../Actions/Remove/Remove.h"
#include "../Actions/Show/Show.h"
#include "../List/ListItems/ListItemEntity.h"
#include "../List/ListName.h"

ArchiveUseCase::ArchiveUseCase(IOService& ioService,
                               Command& command,
                               ListItemService& listItemService,
                               ListService& listService,
                               ConfigService& configService,
                               ThemeService& themeService)
  : ioService(ioService)
  , command(command)
  , listItemService(listItemService)
  , listService(listService)
  , configService(configService)
  , themeService(themeService)
{
}

void
ArchiveUseCase::execute()
{
    ListName listName =
        listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
    Remove remove(ioService, command, listItemService);
    remove.archive(listName);

    Show show(ioService, listService, listItemService, themeService);

    std::vector<ListItemEntity> listItems = listItemService.get(listName);
    try {
        show.print(listItems, listName);
    } catch (std::exception& e) {
        ioService.br();
        ioService.error(e.what());
        ioService.br();
    }
}
