#ifndef EDITITEMACTION_H
#define EDITITEMACTION_H

#include "../../Command/Command.h"
#include "../../Command/CommandService.h"
#include "../../Helpers/DateHelpers.h"
#include "../../Helpers/StringHelpers.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"

class EditItemAction
{
  public:
    EditItemAction(IOService& ioService, CommandService& commandService, ListItemService& listItemService);
    void execute(Command& command, ListName& listName);

  private:
    IOService& ioService;
    CommandService& commandService;
    ListItemService& listItemService;
    static bool isCommandIncludePriority(const std::vector<std::string>& arguments);
};

#endif // ADD_H
