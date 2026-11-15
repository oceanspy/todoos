#include "CopyListUseCase.h"
#include "../Actions/ListAction/CopyListAction.h"
#include "../Actions/ListAction/ShowListAction.h"

CopyListUseCase::CopyListUseCase(IOService& ioService,
                                 ListService& listService,
                                 ListItemService& listItemService,
                                 FileStorageService& fileStorageService,
                                 ConfigService& configService,
                                 ThemeService& themeService,
                                 DescriptionService& descriptionService)
  : ioService(ioService)
  , listService(listService)
  , listItemService(listItemService)
  , fileStorageService(fileStorageService)
  , configService(configService)
  , themeService(themeService)
  , descriptionService(descriptionService)
{
}

void
CopyListUseCase::execute(Command& command)
{
    CopyListAction action(ioService, listService, listItemService, fileStorageService, descriptionService);
    action.execute(command);

    ShowListAction showAction(ioService, listService, configService, themeService);
    showAction.execute(command);
}
