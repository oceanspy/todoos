#include "MoveItemAction.h"

MoveItemAction::MoveItemAction(IOService& ioService, ListService& listService, ListItemService& listItemService)
  : ioService(ioService)
  , listService(listService)
  , listItemService(listItemService)
{
}

std::string
MoveItemAction::execute(Command& command, ListName& listName)
{
    const std::vector<std::string> arguments = command.getArguments();

    if (arguments.size() < 2) {
        ioService.br();
        ioService.error("Please provide the list name where to move the element and its ID.");
        ioService.br();
        return "";
    }

    std::string newListStr = arguments.at(0);
    std::vector<std::string> ids(arguments.begin() + 1, arguments.end());
    ids.erase(unique(ids.begin(), ids.end()), ids.end());

    ListName newListName = listService.createListName(newListStr);

    if (listName.getName() == newListName.getName()) {
        ioService.br();
        ioService.error("Item is already in list " + newListName.getName() + ".");
        ioService.br();
        return "";
    }

    ioService.br();
    for (const auto& id : ids) {
        try {
            ListItemEntity listItemEntity(listName);
            try {
                listItemEntity = listItemService.find(id, newListName);
            } catch (std::exception& e) {
                //
            }

            if (!(*listItemEntity.getId()).empty() && !command.hasOption("force")) {
                std::string answer = ioService.ask("Item " + id + " already exists in list " +
                                                   newListName.getName() + ". Do you want to overwrite it? (y/n) ");
                if (answer != "y" && answer != "yes") {
                    ioService.br();
                    ioService.info("Item " + id + " was not moved from list " + listName.getName() + " to list " +
                                   newListName.getName() + ".");
                    ioService.br();
                    return "";
                }
            }

            listItemService.move(id, listName, newListName);
            ioService.success("Item " + id + " moved from " + listName.getName() + " to list " +
                              newListName.getName() + ".");
        } catch (std::exception& e) {
            ioService.error("Item " + id + " could not be moved from list " + listName.getName() + " to " +
                            newListName.getName() + ".");
            return "";
        }
    }
    ioService.br();

    return newListStr;
}
