#pragma once

#include "../Command/Command.h"
#include "../Config/ConfigService.h"
#include "../IOService/IOService.h"

class CurrentListUseCase
{
  public:
    CurrentListUseCase(IOService& ioService, ConfigService& configService);
    void execute(Command& command);

  private:
    IOService& ioService;
    ConfigService& configService;
};
