#ifndef COPYLISTACTION_H
#define COPYLISTACTION_H

#include "../../Command/Command.h"
#include "../../FileStorage/FileStorageService.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"
#include "../../List/ListService.h"

class CopyListAction
{
  public:
    CopyListAction(IOService& ioService,
                   ListService& listService,
                   ListItemService& listItemService,
                   FileStorageService& fileStorageService);
    void execute(Command& command);

  private:
    IOService& ioService;
    ListService& listService;
    ListItemService& listItemService;
    FileStorageService& fileStorageService;
    static std::string removeSpaceFromListName(std::string listName);
    static std::string addSpaceToListName(std::string listName);
};

#endif // COPYLISTACTION_H
