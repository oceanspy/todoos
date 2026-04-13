#include "AddListAction.h"

AddListAction::AddListAction(IOService& ioService,
                             ListService& listService,
                             FileStorageService& fileStorageService)
  : ioService(ioService)
  , listService(listService)
  , fileStorageService(fileStorageService)
{
}

void
AddListAction::execute(Command& command)
{
    std::vector<std::string> arguments = command.getArguments();

    if (arguments.empty()) {
        ioService.br();
        ioService.error("Please provide a name for the list item.");
        ioService.br();
        return;
    }

    std::string name = removeSpaceFromListName(makeListNameFromArguments(arguments));

    try {
        listService.add(name);
        listService.find(name);
        fileStorageService.createNewListFile(name);
        listService.use(name);
    } catch (std::invalid_argument& e) {
        ioService.br();
        ioService.error("Error while creating the file -- is list name correct?");
        ioService.br();
        return;
    }

    ioService.br();
    ioService.success("New list: " + name + " created.");
    ioService.br();
}

std::string
AddListAction::makeListNameFromArguments(std::vector<std::string>& arguments)
{
    std::string name;
    for (int i = 0; i < (int)arguments.size(); i++) {
        if (i == 0) {
            name += arguments.at(i);
            continue;
        }
        name += "_" + arguments.at(i);
    }
    return name;
}

std::string
AddListAction::removeSpaceFromListName(std::string listName)
{
    std::replace(listName.begin(), listName.end(), ' ', '_');
    return listName;
}
