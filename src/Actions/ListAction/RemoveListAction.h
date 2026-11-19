#ifndef REMOVELISTACTION_H
#define REMOVELISTACTION_H

#include "../../Command/Command.h"
#include "../../Config/ConfigService.h"
#include "../../IOService/IOService.h"
#include "../../List/ListService.h"

class RemoveListAction
{
  public:
    RemoveListAction(IOService& ioService, ListService& listService, ConfigService& configService);
    void execute(Command& command);

  private:
    IOService& ioService;
    ListService& listService;
    ConfigService& configService;
    static std::string removeSpaceFromListName(std::string listName);
};

#endif // REMOVELISTACTION_H
