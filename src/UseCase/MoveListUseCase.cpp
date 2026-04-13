#include "MoveListUseCase.h"
#include "../Actions/ListAction/RenameListAction.h"
#include "../Actions/ListAction/ShowListAction.h"

MoveListUseCase::MoveListUseCase(IOService& ioService,
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
MoveListUseCase::execute(Command& command)
{
    RenameListAction action(ioService, listService, fileStorageService, configService);
    action.execute(command);

    ShowListAction showAction(ioService, listService, configService, themeService);
    showAction.execute(command);
}
