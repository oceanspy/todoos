#ifndef RENAMELISTACTION_H
#define RENAMELISTACTION_H

#include "../../Command/Command.h"
#include "../../Config/ConfigService.h"
#include "../../IOService/IOService.h"
#include "../../List/ListService.h"

class RenameListAction
{
  public:
    RenameListAction(IOService& ioService, ListService& listService, ConfigService& configService);
    void execute(Command& command);

  private:
    IOService& ioService;
    ListService& listService;
    ConfigService& configService;
    static std::string removeSpaceFromListName(std::string listName);
    static std::string addSpaceToListName(std::string listName);
};

#endif // RENAMELISTACTION_H
