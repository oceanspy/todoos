#ifndef APPENDITEMACTION_H
#define APPENDITEMACTION_H

#include "../../Command/Command.h"
#include "../../Command/CommandService.h"
#include "../../Helpers/DateHelpers.h"
#include "../../Helpers/StringHelpers.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"

class AppendItemAction
{
  public:
    AppendItemAction(IOService& ioService, CommandService& commandService, ListItemService& listItemService);
    void execute(Command& command, ListName& listName);

  private:
    IOService& ioService;
    CommandService& commandService;
    ListItemService& listItemService;
    static bool isCommandIncludePriority(const std::vector<std::string>& arguments);
};

#endif // APPENDITEMACTION_H
