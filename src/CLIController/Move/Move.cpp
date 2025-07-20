#include "Move.h"

Move::Move(IOService& ioService,
           ConfigService& configService,
           Command& command,
           CommandService& commandService,
           ListItemService& listItemService)
  : ioService(ioService)
  , configService(configService)
  , command(command)
  , commandService(commandService)
  , listItemService(listItemService)
{
}

std::string
Move::make()
{
    const std::vector<std::string> arguments = command.getArguments();
    std::vector<std::string> adaptedArguments = command.getArguments();

    if (arguments.empty()) {
        ioService.br();
        ioService.error("Please provide the ID of the element to remove.");
        ioService.br();
        return "";
    }

    std::string oldList = configService.getCurrentList();
    std::string newList;

    if (CommandService::isCommand(command, "duplicate")) {
        if (arguments.size() < 1) {
            ioService.br();
            ioService.error("Please provide the ID(s) of the element(s) to duplicate.");
            ioService.br();
            return "";
        }

        newList = oldList;
    } else if (CommandService::isCommand(command, "move-to") || CommandService::isCommand(command, "copy-to")) {
        if (arguments.size() < 2) {
            ioService.br();
            ioService.error("Please provide the list name where to move the element and its ID.");
            ioService.br();
            return "";
        }

        newList = command.getArguments().at(0);
        adaptedArguments.erase(adaptedArguments.begin());
    }

    std::vector<std::string> ids = adaptedArguments;

    // clean the double ids
    ids.erase(unique(ids.begin(), ids.end()), ids.end());

    ioService.br();
    for (const auto& id : ids) {
        try {
            if (CommandService::isCommand(command, "copy-to")) {
                ListItemEntity listItemEntity;
                try {
                    listItemEntity = listItemService.load(newList).find(id);
                } catch (std::exception& e) {
                    //
                }

                if (!(*listItemEntity.getId()).empty() && !command.hasOption("force")) {
                    std::string answer = ioService.ask("Item " + id + " already exists in list " + newList +
                                                       ". Do you want to overwrite it? (y/n) ");
                    if (answer != "y" && answer != "yes") {
                        ioService.br();
                        ioService.info("Item " + id + " was not copied from list " + oldList + " to list " + newList +
                                       ".");
                        ioService.br();
                        return "";
                    }
                }

                try {
                    listItemService.copy(id, configService.getCurrentList(), newList);
                } catch (std::exception& e) {
                    ioService.error("Item " + id + " could not be copied from list " + oldList + " to list " + newList +
                                    ".");
                    ioService.info(e.what());
                    return "";
                }

                ioService.success("Item " + id + " copied from " + oldList + " to list " + newList + ".");
            } else if (CommandService::isCommand(command, "move-to")) {
                if (configService.getCurrentList() == newList) {
                    ioService.error("Item " + id + " is already in list " + newList + ".");
                    return "";
                }

                ListItemEntity listItemEntity;

                try {
                    listItemEntity = listItemService.load(newList).find(id);
                } catch (std::exception& e) {
                    //
                }

                if (!(*listItemEntity.getId()).empty() && !command.hasOption("force")) {
                    std::string answer = ioService.ask("Item " + id + " already exists in list " + newList +
                                                       ". Do you want to overwrite it? (y/n) ");
                    if (answer != "y" && answer != "yes") {
                        ioService.br();
                        ioService.info("Item " + id + " was not moved from list " + oldList + " to list " + newList +
                                       ".");
                        ioService.br();
                        return "";
                    }
                }

                try {
                    listItemService.move(id, configService.getCurrentList(), newList);
                } catch (std::exception& e) {
                    ioService.error("Item " + id + " could not be moved from list " + oldList + " to list " + newList +
                                    ".");
                    ioService.info(e.what());
                    return "";
                }

                ioService.success("Item " + id + " moved from " + oldList + " to list " + newList + ".");
            } else if (CommandService::isCommand(command, "duplicate")) {
                try {
                    listItemService.duplicate(id, configService.getCurrentList());
                } catch (std::exception& e) {
                    ioService.error("Item " + id + " could not be copied from list " + oldList + " to list " + newList +
                                    ".");
                    ioService.info(e.what());
                    return "";
                }

                ioService.success("Item " + id + " copied from " + oldList + " to list " + newList + ".");
            }
        } catch (std::exception& e) {
            ioService.error("Item " + id + " could not be moved or copied from list " + oldList + " to " + newList +
                            ".");
            return "";
        }
    }
    ioService.br();

    return newList;
}
