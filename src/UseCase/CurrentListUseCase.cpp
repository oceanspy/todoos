#include "CurrentListUseCase.h"
#include <algorithm>

CurrentListUseCase::CurrentListUseCase(IOService& ioService, ConfigService& configService)
  : ioService(ioService)
  , configService(configService)
{
}

void
CurrentListUseCase::execute(Command& command)
{
    std::string name = configService.getUsedListNameStr();
    std::replace(name.begin(), name.end(), '_', ' ');
    ioService.print(name);
}
