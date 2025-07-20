#ifndef ADD_H
#define ADD_H

#include "../../Command/Command.h"
#include "../../Command/CommandService.h"
#include "../../Helpers/DateHelpers.h"
#include "../../Helpers/StringHelpers.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"

class ListItemActions
{
  public:
    ListItemActions(IOService& ioService,
                    Command& command,
                    CommandService& commandService,
                    ListItemService& listItemService);
    void make();

  private:
    IOService& ioService;
    Command& command;
    CommandService& commandService;
    ListItemService& listItemService;
    static bool isCommandIncludePriority(const std::vector<std::string>& arguments);
    static std::string calculateValue(std::vector<std::string> arguments);
};

#endif // ADD_H
