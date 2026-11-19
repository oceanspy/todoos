#ifndef ADDLISTACTION_H
#define ADDLISTACTION_H

#include "../../Command/Command.h"
#include "../../IOService/IOService.h"
#include "../../List/ListService.h"

class AddListAction
{
  public:
    AddListAction(IOService& ioService, ListService& listService);
    void execute(Command& command);

  private:
    IOService& ioService;
    ListService& listService;
    static std::string makeListNameFromArguments(std::vector<std::string>& arguments);
    static std::string removeSpaceFromListName(std::string listName);
};

#endif // ADDLISTACTION_H
