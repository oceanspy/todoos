#include "CopyItemAction.h"

CopyItemAction::CopyItemAction(IOService& ioService,
                               ListService& listService,
                               ListItemService& listItemService,
                               DescriptionService& descriptionService)
  : ioService(ioService)
  , listService(listService)
  , listItemService(listItemService)
  , descriptionService(descriptionService)
{
}

std::string
CopyItemAction::execute(Command& command, ListName& listName)
{
    const std::vector<std::string> arguments = command.getArguments();

    if (arguments.empty()) {
        ioService.br();
        ioService.error("Please provide the ID(s) of the element(s) to copy.");
        ioService.br();
        return "";
    }

    std::string newListStr = command.getOption("to");
    if (newListStr.empty()) {
        ioService.br();
        ioService.error("Please provide the target list with --to {list}.");
        ioService.br();
        return "";
    }

    std::vector<std::string> ids = arguments;
    ids.erase(unique(ids.begin(), ids.end()), ids.end());

    ListName newListName = listService.createListName(newListStr);

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
                std::string answer = ioService.ask("Item " + id + " already exists in list " + newListName.getName() +
                                                   ". Do you want to overwrite it? (y/n) ");
                if (answer != "y" && answer != "yes") {
                    ioService.br();
                    ioService.info("Item " + id + " was not copied from list " + listName.getName() + " to list " +
                                   newListName.getName() + ".");
                    ioService.br();
                    return "";
                }
            }

            listItemService.copy(id, listName, newListName);
            descriptionService.copy(id, listName, newListName);

            ioService.success("Item " + id + " copied from " + listName.getName() + " to list " +
                              newListName.getName() + ".");
        } catch (std::exception& e) {
            ioService.error("Item " + id + " could not be copied from list " + listName.getName() + " to " +
                            newListName.getName() + ".");
            ioService.info(e.what());
            return "";
        }
    }
    ioService.br();

    return newListStr;
}
