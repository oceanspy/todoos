#ifndef CONFIG_H
#define CONFIG_H

#include "../../Command/Command.h"
#include "../../Command/CommandService.h"
#include "../../Config/ConfigService.h"
#include "../../IOService/IOService.h"
#include "../../List/ListService.h"

class Config
{
  public:
    Config(IOService& ioService,
           Command& command,
           CommandService& commandService,
           ConfigService& configService,
           ListService& listService);
    void make();

  private:
    IOService& ioService;
    Command& command;
    CommandService& commandService;
    ConfigService& configService;
    ListService& listService;

    void showAll();
};

#endif // CONFIG_H
