#ifndef COPYLISTACTION_H
#define COPYLISTACTION_H

#include "../../Command/Command.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"
#include "../../List/ListService.h"

class CopyListAction
{
  public:
    CopyListAction(IOService& ioService, ListService& listService, ListItemService& listItemService);
    void execute(Command& command);

  private:
    IOService& ioService;
    ListService& listService;
    ListItemService& listItemService;
    static std::string removeSpaceFromListName(std::string listName);
    static std::string addSpaceToListName(std::string listName);
};

#endif // COPYLISTACTION_H
