#ifndef REMOVEITEMACTION_H
#define REMOVEITEMACTION_H

#include "../../Command/Command.h"
#include "../../FileDataStorageRepositories/DescriptionRepository.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"

class RemoveItemAction
{
  public:
    RemoveItemAction(IOService& ioService, ListItemService& listItemService, DescriptionRepository& descriptionRepository);
    void execute(Command& command, ListName& listName);

  private:
    IOService& ioService;
    ListItemService& listItemService;
    DescriptionRepository& descriptionRepository;
};

#endif // REMOVEITEMACTION_H
