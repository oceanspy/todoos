#ifndef REMOVEITEMACTION_H
#define REMOVEITEMACTION_H

#include "../../Command/Command.h"
#include "../../Describe/DescriptionRepositoryInterface.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"

class RemoveItemAction
{
  public:
    RemoveItemAction(IOService& ioService,
                     ListItemService& listItemService,
                     DescriptionRepositoryInterface& descriptionRepository);
    void execute(Command& command, ListName& listName);

  private:
    IOService& ioService;
    ListItemService& listItemService;
    DescriptionRepositoryInterface& descriptionRepository;
};

#endif // REMOVEITEMACTION_H
