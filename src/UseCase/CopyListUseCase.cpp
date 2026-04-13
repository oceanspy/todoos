#include "CopyListUseCase.h"
#include "../Actions/ListAction/CopyListAction.h"
#include "../Actions/ListAction/ShowListAction.h"

CopyListUseCase::CopyListUseCase(IOService& ioService,
                                 ListService& listService,
                                 ListItemService& listItemService,
                                 FileStorageService& fileStorageService,
                                 ConfigService& configService,
                                 ThemeService& themeService)
  : ioService(ioService)
  , listService(listService)
  , listItemService(listItemService)
  , fileStorageService(fileStorageService)
  , configService(configService)
  , themeService(themeService)
{
}

void
CopyListUseCase::execute(Command& command)
{
    CopyListAction action(ioService, listService, listItemService, fileStorageService);
    action.execute(command);

    ShowListAction showAction(ioService, listService, configService, themeService);
    showAction.execute(command);
}
