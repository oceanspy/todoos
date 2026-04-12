#include "DeadlineItemAction.h"

DeadlineItemAction::DeadlineItemAction(IOService& ioService,
                                       CommandService& commandService,
                                       ListItemService& listItemService)
  : ioService(ioService)
  , commandService(commandService)
  , listItemService(listItemService)
{
}

void
DeadlineItemAction::execute(Command& command, ListName& listName)
{
    const std::vector<std::string> arguments = command.getArguments();
    std::vector<std::string> itemValueFromArguments = command.getArguments();

    if (arguments.size() < 2) {
        ioService.br();
        ioService.error("Please provide the ID of the item and the deadline date (expl: 2031.11.31).");
        ioService.br();
        return;
    }
    std::string stringDate = itemValueFromArguments.at(0);
    itemValueFromArguments.erase(itemValueFromArguments.begin());

    std::vector<std::string> ids = itemValueFromArguments;

    // clean the double ids
    ids.erase(unique(ids.begin(), ids.end()), ids.end());

    ioService.br();
    for (auto& id : ids) {
        if (!StringHelpers::isAlnum(id)) {
            ioService.error("ID is not valid.");
            continue;
        }

        try {
            time_t dueAt = DateHelpers::relativeDateToTimestamp(stringDate);
            listItemService.editDeadline(id, listName, dueAt);
            ioService.success("Deadline of " + id + " updated.");
        } catch (std::exception& e) {
            ioService.error(e.what());
            continue;
        }
    }
    ioService.br();
}
