#ifndef DUPLICATEITEMACTION_H
#define DUPLICATEITEMACTION_H

#include "../../Command/Command.h"
#include "../../Describe/DescriptionService.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"
#include "../../List/ListName.h"
#include "../../List/ListService.h"

class DuplicateItemAction
{
  public:
    DuplicateItemAction(IOService& ioService,
                        ListService& listService,
                        ListItemService& listItemService,
                        DescriptionService& descriptionService);
    std::string execute(Command& command, ListName& listName);

  private:
    IOService& ioService;
    ListService& listService;
    ListItemService& listItemService;
    DescriptionService& descriptionService;
};

#endif // DUPLICATEITEMACTION_H
