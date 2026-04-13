#include "SwitchListUseCase.h"
#include "../Actions/ShowAction/ShowAction.h"
#include "../List/ListItems/ListItemEntity.h"
#include "../List/ListName.h"

SwitchListUseCase::SwitchListUseCase(IOService& ioService,
                                     CommandService& commandService,
                                     ListService& listService,
                                     ListItemService& listItemService,
                                     FileStorageService& fileStorageService,
                                     ConfigService& configService,
                                     ThemeService& themeService)
  : ioService(ioService)
  , commandService(commandService)
  , listService(listService)
  , listItemService(listItemService)
  , fileStorageService(fileStorageService)
  , configService(configService)
  , themeService(themeService)
{
}

void
SwitchListUseCase::execute(Command& command)
{
    std::string listName =
        command.getArguments().empty() ? configService.getDefaultList() : command.getArguments().at(0);

    if (!listService.use(listName)) {
        ioService.br();
        ioService.error("List \"" + listName + "\" not found.");
        ioService.br();
        return;
    }

    ioService.br();
    ioService.success("Now using list: " + listName);
    ioService.br();

    ListName currentListName =
        listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
    ShowAction show(ioService, listService, listItemService, themeService);

    std::vector<ListItemEntity> listItems = listItemService.get(currentListName);

    try {
        show.print(listItems, currentListName);
    } catch (std::exception& e) {
        ioService.br();
        ioService.error(e.what());
        ioService.br();
    }
}
