#include "RenameListUseCase.h"
#include "../Actions/ListAction/RenameListAction.h"
#include "../Actions/ListAction/ShowListAction.h"

RenameListUseCase::RenameListUseCase(IOService& ioService,
                                     ListService& listService,
                                     FileStorageService& fileStorageService,
                                     ConfigService& configService,
                                     ThemeService& themeService,
                                     DescriptionService& descriptionService)
  : ioService(ioService)
  , listService(listService)
  , fileStorageService(fileStorageService)
  , configService(configService)
  , themeService(themeService)
  , descriptionService(descriptionService)
{
}

void
RenameListUseCase::execute(Command& command)
{
    RenameListAction action(ioService, listService, fileStorageService, configService, descriptionService);
    action.execute(command);

    ShowListAction showAction(ioService, listService, configService, themeService);
    showAction.execute(command);
}
