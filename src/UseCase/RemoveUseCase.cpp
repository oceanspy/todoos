#include "RemoveUseCase.h"
#include "../Actions/ListItemAction/RemoveItemAction.h"
#include "../Actions/ShowAction/ShowAction.h"
#include "../List/ListItems/ListItemEntity.h"
#include "../List/ListName.h"

RemoveUseCase::RemoveUseCase(IOService& ioService,
                             ListItemService& listItemService,
                             ListService& listService,
                             ConfigService& configService,
                             ThemeService& themeService,
                             DescriptionRepository& descriptionRepository)
  : ioService(ioService)
  , listItemService(listItemService)
  , listService(listService)
  , configService(configService)
  , themeService(themeService)
  , descriptionRepository(descriptionRepository)
{
}

void
RemoveUseCase::execute(Command& command, ListName& currentList)
{
    RemoveItemAction remove(ioService, listItemService, descriptionRepository);
    remove.execute(command, currentList);

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
