#ifndef COPYITEMACTION_H
#define COPYITEMACTION_H

#include "../../Command/Command.h"
#include "../../Command/CommandService.h"
#include "../../Describe/DescriptionService.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"
#include "../../List/ListName.h"
#include "../../List/ListService.h"

class CopyItemAction
{
  public:
    CopyItemAction(IOService& ioService,
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

#endif // COPYITEMACTION_H
