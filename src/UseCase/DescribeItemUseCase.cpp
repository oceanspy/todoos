#include "DescribeItemUseCase.h"
#include "../Actions/ListItemAction/AddItemAction.h"
#include "../Actions/ShowAction/ShowAction.h"
#include "../List/ListItems/ListItemEntity.h"
#include "../List/ListName.h"

DescribeItemUseCase::DescribeItemUseCase(IOService& ioService,
                                         CommandService& commandService,
                                         ListItemService& listItemService,
                                         ListService& listService,
                                         ConfigService& configService,
                                         ThemeService& themeService)
  : ioService(ioService)
  , commandService(commandService)
  , listItemService(listItemService)
  , listService(listService)
  , configService(configService)
  , themeService(themeService)
{
}

void
DescribeItemUseCase::execute(Command& command, ListName& currentList)
{
    std::string listItemId = command.getName();

    ListItemEntity listItem = listItemService.find(listItemId, currentList);

    // load existing content

    // build temporary file
    // # Todoos
    // # ----
    // # ID: {id}
    // # List Name: {currentList}
    // # Item: {title}

    // save new item title to db

    // save to file if description not empty

    ioService.br();
    ioService.success("described! WIP");
    ioService.br();

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
