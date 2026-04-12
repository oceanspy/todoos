#include "ListUseCase.h"
#include "../Actions/ListAction/ListAction.h"

ListUseCase::ListUseCase(IOService& ioService,
                         Command& command,
                         CommandService& commandService,
                         ListService& listService,
                         ListItemService& listItemService,
                         FileStorageService& fileStorageService,
                         ConfigService& configService,
                         ThemeService& themeService)
  : ioService(ioService)
  , command(command)
  , commandService(commandService)
  , listService(listService)
  , listItemService(listItemService)
  , fileStorageService(fileStorageService)
  , configService(configService)
  , themeService(themeService)
{
}

void
ListUseCase::execute()
{
    ListAction list(ioService,
                    command,
                    commandService,
                    listService,
                    listItemService,
                    fileStorageService,
                    configService,
                    themeService);
    list.make();
}
