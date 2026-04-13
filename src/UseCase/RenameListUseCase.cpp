#include "RenameListUseCase.h"
#include "../Actions/ListAction/RenameListAction.h"
#include "../Actions/ListAction/ShowListAction.h"

RenameListUseCase::RenameListUseCase(IOService& ioService,
                                     ListService& listService,
                                     FileStorageService& fileStorageService,
                                     ConfigService& configService,
                                     ThemeService& themeService)
  : ioService(ioService)
  , listService(listService)
  , fileStorageService(fileStorageService)
  , configService(configService)
  , themeService(themeService)
{
}

void
RenameListUseCase::execute(Command& command)
{
    RenameListAction action(ioService, listService, fileStorageService, configService);
    action.execute(command);

    ShowListAction showAction(ioService, listService, configService, themeService);
    showAction.execute(command);
}
