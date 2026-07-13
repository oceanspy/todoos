#include "ArchiveUseCase.h"
#include "../Actions/ListItemAction/ArchiveItemAction.h"
#include "../Actions/ShowAction/ShowAction.h"
#include "../List/ListItems/ListItemEntity.h"
#include "../List/ListName.h"

ArchiveUseCase::ArchiveUseCase(IOService& ioService,
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
ArchiveUseCase::execute(Command& command, ListName& currentList)
{
    ArchiveItemAction archive(ioService, listItemService);
    archive.execute(command, currentList);

    ShowAction show(ioService, listService, listItemService, themeService);

    std::vector<ListItemEntity> listItems = listItemService.get(currentList);
    try {
        show.execute(listItems, currentList);
    } catch (std::exception& e) {
        ioService.br();
        ioService.error(e.what());
        ioService.br();
    }
}
