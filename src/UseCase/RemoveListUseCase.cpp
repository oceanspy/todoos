#include "RemoveListUseCase.h"
#include "../Actions/ListAction/RemoveListAction.h"
#include "../Actions/ListAction/ShowListAction.h"

RemoveListUseCase::RemoveListUseCase(IOService& ioService,
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
RemoveListUseCase::execute(Command& command)
{
    RemoveListAction action(ioService, listService, fileStorageService, configService);
    action.execute(command);

    ShowListAction showAction(ioService, listService, configService, themeService);
    showAction.execute(command);
}
