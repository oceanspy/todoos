#include "SwitchListUseCase.h"
#include "../Actions/ListAction/ListAction.h"
#include "../Actions/ShowAction/ShowAction.h"
#include "../List/ListItems/ListItemEntity.h"
#include "../List/ListName.h"

SwitchListUseCase::SwitchListUseCase(IOService& ioService,
                                     Command& command,
                                     CommandService& commandService,
                                     ListService& listService,
                                     ListItemService& listItemService,
                                     FileStorageService& fileStorageService,
                                     ConfigService& configService,
                                     ThemeService& themeService)
  : ioService(ioService)
  , command(command)
  , commandService(commandService)
  , listService(listService)
  , listItemService(listItemService)
  , fileStorageService(fileStorageService)
  , configService(configService)
  , themeService(themeService)
{
}

void
SwitchListUseCase::execute()
{
    ListAction list(ioService,
                    command,
                    commandService,
                    listService,
                    listItemService,
                    fileStorageService,
                    configService,
                    themeService);

    list.use();

    ListName listName =
        listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
    ShowAction show(ioService, listService, listItemService, themeService);

    std::vector<ListItemEntity> listItems = listItemService.get(listName);

    try {
        show.print(listItems, listName);
    } catch (std::exception& e) {
        ioService.br();
        ioService.error(e.what());
        ioService.br();
    }
}
