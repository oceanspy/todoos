#include "ListUseCase.h"
#include "../Actions/ListActions/ListActions.h"

ListUseCase::ListUseCase(IOService& ioService,
                          Command& command,
                          CommandService& commandService,
                          ListService& listService,
                          ListItemService& listItemService,
                          FileStorageService& fileStorageService,
                          ConfigService& configService,
                          CLIThemeService& cliThemeService)
  : ioService(ioService)
  , command(command)
  , commandService(commandService)
  , listService(listService)
  , listItemService(listItemService)
  , fileStorageService(fileStorageService)
  , configService(configService)
  , cliThemeService(cliThemeService)
{
}

void
ListUseCase::execute()
{
    ListActions list(ioService,
                     command,
                     commandService,
                     listService,
                     listItemService,
                     fileStorageService,
                     configService,
                     cliThemeService);
    list.make();
}
