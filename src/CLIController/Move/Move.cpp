#include "Move.h"

Move::Move(IOService& ioService,
           Command& command,
           CommandService& commandService,
           ListService& listService,
           ListItemService& listItemService)
  : ioService(ioService)
  , command(command)
  , commandService(commandService)
  , listService(listService)
  , listItemService(listItemService)
{
}

std::string
Move::make(ListName& listName)
{
    const std::vector<std::string> arguments = command.getArguments();
    std::vector<std::string> adaptedArguments = command.getArguments();

    if (arguments.empty()) {
        ioService.br();
        ioService.error("Please provide the ID of the element to remove.");
        ioService.br();
        return "";
    }

    std::string newListStr;

    if (CommandService::isCommand(command, "duplicate")) {
        if (arguments.size() < 1) {
            ioService.br();
            ioService.error("Please provide the ID(s) of the element(s) to duplicate.");
            ioService.br();
            return "";
        }

        newListStr = listName.getName();
    } else if (CommandService::isCommand(command, "move-to") || CommandService::isCommand(command, "copy-to")) {
        if (arguments.size() < 2) {
            ioService.br();
            ioService.error("Please provide the list name where to move the element and its ID.");
            ioService.br();
            return "";
        }

        newListStr = command.getArguments().at(0);
        adaptedArguments.erase(adaptedArguments.begin());
    }

    ListName newListName = listService.createListName(newListStr);

    std::vector<std::string> ids = adaptedArguments;

    // clean the double ids
    ids.erase(unique(ids.begin(), ids.end()), ids.end());

    ioService.br();
    for (const auto& id : ids) {
        try {
            if (CommandService::isCommand(command, "copy-to")) {
                ListItemEntity listItemEntity(listName);
                try {
                    listItemEntity = listItemService.find(id, listName);
                } catch (std::exception& e) {
                    //
                }

                if (!(*listItemEntity.getId()).empty() && !command.hasOption("force")) {
                    std::string answer = ioService.ask("Item " + id + " already exists in list " +
                                                       newListName.getName() + ". Do you want to overwrite it? (y/n) ");
                    if (answer != "y" && answer != "yes") {
                        ioService.br();
                        ioService.info("Item " + id + " was not copied from list " + listName.getName() + " to list " +
                                       newListName.getName() + ".");
                        ioService.br();
                        return "";
                    }
                }

                try {
                    listItemService.copy(id, listName, newListName);
                } catch (std::exception& e) {
                    ioService.error("Item " + id + " could not be copied from list " + listName.getName() +
                                    " to list " + newListName.getName() + ".");
                    ioService.info(e.what());
                    return "";
                }

                ioService.success("Item " + id + " copied from " + listName.getName() + " to list " +
                                  newListName.getName() + ".");
            } else if (CommandService::isCommand(command, "move-to")) {
                if (listName.getName() == newListName.getName()) {
                    ioService.error("Item " + id + " is already in list " + newListName.getName() + ".");
                    return "";
                }

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

                try {
                    listItemService.move(id, listName, newListName);
                } catch (std::exception& e) {
                    ioService.error("Item " + id + " could not be moved from list " + listName.getName() + " to list " +
                                    newListName.getName() + ".");
                    ioService.info(e.what());
                    return "";
                }

                ioService.success("Item " + id + " moved from " + listName.getName() + " to list " +
                                  newListName.getName() + ".");
            } else if (CommandService::isCommand(command, "duplicate")) {
                try {
                    listItemService.duplicate(id, listName);
                } catch (std::exception& e) {
                    ioService.error("Item " + id + " could not be copied from list " + listName.getName() +
                                    " to list " + newListName.getName() + ".");
                    ioService.info(e.what());
                    return "";
                }

                ioService.success("Item " + id + " copied from " + listName.getName() + " to list " +
                                  newListName.getName() + ".");
            }
        } catch (std::exception& e) {
            ioService.error("Item " + id + " could not be moved or copied from list " + listName.getName() + " to " +
                            newListName.getName() + ".");
            return "";
        }
    }
    ioService.br();

    return newListStr;
}
