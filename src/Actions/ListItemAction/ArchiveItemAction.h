#ifndef ARCHIVEITEMACTION_H
#define ARCHIVEITEMACTION_H

#include "../../Command/Command.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"

class ArchiveItemAction
{
  public:
    ArchiveItemAction(IOService& ioService, ListItemService& listItemService);
    void execute(Command& command, ListName& listName);

  private:
    IOService& ioService;
    ListItemService& listItemService;
};

#endif // ARCHIVEITEMACTION_H
