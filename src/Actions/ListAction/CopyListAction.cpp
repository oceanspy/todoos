#include "CopyListAction.h"

CopyListAction::CopyListAction(IOService& ioService,
                               ListService& listService,
                               ListItemService& listItemService,
                               FileStorageService& fileStorageService)
  : ioService(ioService)
  , listService(listService)
  , listItemService(listItemService)
  , fileStorageService(fileStorageService)
{
}

void
CopyListAction::execute(Command& command)
{
    std::vector<std::string> arguments = command.getArguments();

    if (arguments.empty()) {
        ioService.error("Please provide the name of the list to copy.");
        return;
    }

    if (arguments.size() < 2) {
        ioService.error("Please provide the name of the list to copy and the new name.");
        return;
    }

    std::string listNameStr = removeSpaceFromListName(arguments.at(0));
    std::string newListNameStr = removeSpaceFromListName(arguments.at(1));

    try {
        listService.find(listNameStr);
    } catch (std::invalid_argument& e) {
        ioService.br();
        ioService.error("List " + addSpaceToListName(listNameStr) + " not found.");
        ioService.br();
        return;
    }

    try {
        listService.find(newListNameStr);
        ioService.br();
        ioService.error("List " + addSpaceToListName(newListNameStr) + " already exist.");
        ioService.br();
        return;
    } catch (std::invalid_argument& e) {
        // we want this
    }

    try {
        fileStorageService.createNewListFile(newListNameStr);
        listService.add(newListNameStr);

        ListName listName = listService.createListName(listNameStr);
        ListName newListName = listService.createListName(newListNameStr);

        std::vector<ListItemEntity> listItems = listItemService.get(listName);
        for (ListItemEntity& listItem : listItems) {
            listItemService.copy(*listItem.getId(), listName, newListName);
        }
    } catch (std::invalid_argument& e) {
        ioService.error("Error while creating the file -- is list name correct?");
        return;
    }

    ioService.br();
    ioService.success("List " + addSpaceToListName(listNameStr) + " copied.");
    ioService.br();
}

std::string
CopyListAction::removeSpaceFromListName(std::string listName)
{
    std::replace(listName.begin(), listName.end(), ' ', '_');
    return listName;
}

std::string
CopyListAction::addSpaceToListName(std::string listName)
{
    std::replace(listName.begin(), listName.end(), '_', ' ');
    return listName;
}
