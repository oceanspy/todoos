#ifndef PREPENDITEMACTION_H
#define PREPENDITEMACTION_H

#include "../../Command/Command.h"
#include "../../Command/CommandService.h"
#include "../../Helpers/DateHelpers.h"
#include "../../Helpers/StringHelpers.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"

class PrependItemAction
{
  public:
    PrependItemAction(IOService& ioService, CommandService& commandService, ListItemService& listItemService);
    void execute(Command& command, ListName& listName);

  private:
    IOService& ioService;
    CommandService& commandService;
    ListItemService& listItemService;
    static bool isCommandIncludePriority(const std::vector<std::string>& arguments);
};

#endif // PREPENDITEMACTION_H
