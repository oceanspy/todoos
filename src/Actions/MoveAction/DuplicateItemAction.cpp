#include "DuplicateItemAction.h"

DuplicateItemAction::DuplicateItemAction(IOService& ioService,
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
DuplicateItemAction::execute(Command& command, ListName& listName)
{
    const std::vector<std::string> arguments = command.getArguments();

    if (arguments.empty()) {
        ioService.br();
        ioService.error("Please provide the ID(s) of the element(s) to duplicate.");
        ioService.br();
        return "";
    }

    std::vector<std::string> ids = arguments;
    ids.erase(unique(ids.begin(), ids.end()), ids.end());

    ioService.br();
    for (const auto& id : ids) {
        try {
            std::string newId = listItemService.duplicate(id, listName);
            descriptionService.duplicate(id, newId, listName);

            ioService.success("Item " + id + " duplicated in list " + listName.getName() + ".");
        } catch (std::exception& e) {
            ioService.error("Item " + id + " could not be duplicated in list " + listName.getName() + ".");
            ioService.info(e.what());
            return "";
        }
    }
    ioService.br();

    return listName.getName();
}
