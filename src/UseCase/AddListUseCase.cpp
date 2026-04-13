#include "AddListUseCase.h"
#include "../Actions/ListAction/AddListAction.h"
#include "../Actions/ListAction/ShowListAction.h"

AddListUseCase::AddListUseCase(IOService& ioService,
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
AddListUseCase::execute(Command& command)
{
    AddListAction action(ioService, listService, fileStorageService);
    action.execute(command);

    ShowListAction showAction(ioService, listService, configService, themeService);
    showAction.execute(command);
}
