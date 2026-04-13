#include "ShowListUseCase.h"
#include "../Actions/ListAction/ShowListAction.h"

ShowListUseCase::ShowListUseCase(IOService& ioService,
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
ShowListUseCase::execute(Command& command)
{
    ShowListAction action(ioService, listService, configService, themeService);
    action.execute(command);
}
