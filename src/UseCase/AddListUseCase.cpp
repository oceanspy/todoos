#include "AddListUseCase.h"
#include "../Actions/ListAction/AddListAction.h"
#include "../Actions/ListAction/ShowListAction.h"

AddListUseCase::AddListUseCase(IOService& ioService,
                               ListService& listService,
                               ConfigService& configService,
                               ThemeService& themeService)
  : ioService(ioService)
  , listService(listService)
  , configService(configService)
  , themeService(themeService)
{
}

void
AddListUseCase::execute(Command& command)
{
    AddListAction action(ioService, listService);
    action.execute(command);

    ShowListAction showAction(ioService, listService, configService, themeService);
    showAction.execute(command);
}
