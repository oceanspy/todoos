#include "ListActions.h"
#include "../../CLIThemes/ThemeAbstract.h"
#include "../../Helpers/BashStyle.h"

ListActions::ListActions(IOService& ioService,
                         Command& command,
                         CommandService& commandService,
                         ListService& listService,
                         ListItemService& listItemService,
                         FileStorageService& fileStorageService,
                         ConfigService& configService,
                         CLIThemeService& cliThemeService)
  : ioService(ioService)
  , command(command)
  , commandService(commandService)
  , listService(listService)
  , listItemService(listItemService)
  , fileStorageService(fileStorageService)
  , configService(configService)
  , cliThemeService(cliThemeService)
{
}

void
ListActions::use()
{
    if (command.getArguments().empty()) {
        if (listService.use(configService.getDefaultList())) {
            ioService.br();
            ioService.success("Now using default list: " + configService.getDefaultList());
            ioService.br();
            return;
        }
        return;
    }

    if (listService.use(removeSpaceFromListName(command.getArguments().at(0)))) {
        ioService.br();
        ioService.success("Now using list: " + addSpaceToListName(command.getArguments().at(0)));
        ioService.br();
        return;
    }

    ioService.br();
    ioService.error("List not found. Aborting.");
    ioService.br();
    return;
}

void
ListActions::make()
{
    Command subCommand = commandService.getSubCommand(command);
    if (subCommand.getName() == "show" || subCommand.getName().empty()) {
        showList();
        return;
    } else if (CommandService::isCommand(subCommand, "add")) {
        addList(subCommand);
        showList();
        return;

    } else if (CommandService::isCommand(subCommand, "remove")) {
        removeList(subCommand);
        showList();
        return;

    } else if (CommandService::isCommand(subCommand, "move-to")) {
        renameList(subCommand);
        showList();
        return;
    } else if (CommandService::isCommand(subCommand, "copy")) {
        copy();
        showList();
        return;
    } else if (CommandService::isCommand(subCommand, "current")) {
        ioService.printWithoutLineBreak(addSpaceToListName(configService.getCurrentList()));
        return;
    }

    throw std::invalid_argument("Invalid config action.");
}

void
ListActions::showList()
{
    ThemeAbstract* theme = cliThemeService.getTheme();
    theme->printATitle("Lists available", "");

    std::vector<ListEntity> listItems = listService.get();
    int i = 0;
    std::vector<std::string> lines;
    for (ListEntity& listItem : listItems) {
        std::string line;
        ++i;
        line = " " + std::to_string(i) + ". ";
        line += buildName(listItem);
        lines.push_back(line);
    }

    // TODO: Create in theme a method to print a raw list
    std::string title = "    NAME";
    theme->printFullLine(GRAY);
    theme->printAListTitle({ title }, { 40 });
    theme->printAList(lines);
}

void
ListActions::addList(Command subCommand)
{
    std::vector<std::string> arguments = subCommand.getArguments();

    if (arguments.empty()) {
        ioService.br();
        ioService.error("Please provide a name for the list item.");
        ioService.br();
        return;
    }

    std::string name = removeSpaceFromListName(makeListNameFromArguments(arguments));
    std::string theme = "default";
    std::string sorting = "default";
    std::string hidden = "false";

    try {
        listService.add(name);
        ListEntity listEntity = listService.find(name);
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

void
ListActions::removeList(Command subCommand)
{
    std::vector<std::string> arguments = subCommand.getArguments();

    if (arguments.empty()) {
        ioService.error("Please provide a name for the list item.");
        return;
    }
    std::string listName = removeSpaceFromListName(arguments.at(0));
    std::string goBackList =
        configService.getCurrentList() == listName ? configService.getDefaultList() : configService.getCurrentList();

    try {
        listService.remove(listName);
        fileStorageService.moveFileTo(listName, "._backup_del_" + listName);
        listService.use(goBackList);
    } catch (std::invalid_argument& e) {
        ioService.br();
        ioService.error("Error while removing the file -- is list name correct?");
        ioService.br();
        return;
    }

    ioService.br();
    ioService.success("List " + listName + " removed.");
    ioService.br();
}

void
ListActions::renameList(Command subCommand)
{
    std::vector<std::string> arguments = subCommand.getArguments();

    if (arguments.size() < 2) {
        ioService.error("Please provide the name of the list you want to rename and the new name.");
        return;
    }
    std::string oldListName = removeSpaceFromListName(arguments.at(0));
    std::string newListName = removeSpaceFromListName(arguments.at(1));
    std::string goBackList =
        configService.getCurrentList() == oldListName ? newListName : configService.getCurrentList();

    try {
        fileStorageService.moveFileTo(oldListName, newListName);
        listService.edit(oldListName, newListName);
        listService.use(goBackList);
    } catch (std::invalid_argument& e) {
        ioService.br();
        ioService.error("Error while renaming the file");
        ioService.br();
    }

    ioService.br();
    ioService.success("List " + addSpaceToListName(oldListName) + " moved to " + addSpaceToListName(newListName) + ".");
    ioService.br();
}

std::string
ListActions::buildName(const ListEntity& listEntity)
{
    std::string name = addSpaceToListName(*listEntity.getName());

    if (configService.getDefaultList() == *listEntity.getName()) {
        name += " (default)";
    }

    if (configService.getCurrentList() == *listEntity.getName()) {
        name += " (in use)";
        name = StringHelpers::colorize(name, BG_GREEN);
    } else if (configService.getDefaultList() == *listEntity.getName()) {
        name = StringHelpers::colorize(name, BG_BLUE);
    }

    return StringHelpers::adjustStringLength(name, 40);
}

void
ListActions::copy()
{
    if (command.getArguments().empty()) {
        ioService.error("Please provide the name of the list to copy.");
        return;
    }

    if (command.getArguments().size() <= 2) {
        ioService.error("Please provide the name of the list to copy and the new name.");
        return;
    }

    std::string listName = removeSpaceFromListName(command.getArguments().at(1));
    std::string newListName = removeSpaceFromListName(command.getArguments().at(2));
    try {
        listService.find(listName);
    } catch (std::invalid_argument& e) {
        ioService.br();
        ioService.error("List " + addSpaceToListName(listName) + " not found.");
        ioService.br();
        return;
    }

    try {
        listService.find(newListName);
        ioService.br();
        ioService.error("List " + addSpaceToListName(newListName) + " already exist.");
        ioService.br();
        return;
    } catch (std::invalid_argument& e) {
        // we want this
    }

    try {
        ListEntity newList;
        newList.setName(newListName);
        fileStorageService.createNewListFile(newListName);
        listService.add(newListName);
        std::vector<ListItemEntity> listItems = listItemService.load(listName).get();
        for (ListItemEntity& listItem : listItems) {
            listItemService.copy(*listItem.getId(), listName, newListName);
        }
    } catch (std::invalid_argument& e) {
        ioService.error("Error while creating the file -- is list name correct?");
        return;
    }

    ioService.br();
    ioService.success("List " + addSpaceToListName(listName) + " copied.");
    ioService.br();
}

std::string
ListActions::makeListNameFromArguments(std::vector<std::string>& arguments)
{
    std::string name;
    for (int i = 0; i < arguments.size(); i++) {
        if (i == 0) {
            name += arguments.at(i);
            continue;
        }
        name += "_" + arguments.at(i);
    }
    return name;
}

std::string
ListActions::removeSpaceFromListName(std::string listName)
{
    std::replace(listName.begin(), listName.end(), ' ', '_');
    return listName;
}

std::string
ListActions::addSpaceToListName(std::string listName)
{
    std::replace(listName.begin(), listName.end(), '_', ' ');
    return listName;
}
