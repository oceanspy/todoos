#ifndef DUPLICATEITEMACTION_H
#define DUPLICATEITEMACTION_H

#include "../../Command/Command.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"
#include "../../List/ListName.h"
#include "../../List/ListService.h"

class DuplicateItemAction
{
  public:
    DuplicateItemAction(IOService& ioService, ListService& listService, ListItemService& listItemService);
    std::string execute(Command& command, ListName& listName);

  private:
    IOService& ioService;
    ListService& listService;
    ListItemService& listItemService;
};

#endif // DUPLICATEITEMACTION_H
