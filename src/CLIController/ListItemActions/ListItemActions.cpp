#include "ListItemActions.h"

ListItemActions::ListItemActions(IOService& ioService, Command& command, CommandService& commandService, ListItemService& listItemService)
    : ioService(ioService), command(command), commandService(commandService), listItemService(listItemService)
{

}

void ListItemActions::make()
{
    const std::vector <std::string> arguments = command.getArguments();
    std::vector <std::string> itemValueFromArguments = command.getArguments();

    if (CommandService::isCommand(command, "add"))
    {
        if (arguments.empty())
        {
            ioService.br();
            ioService.error("Please provide the value you want to add.");
            ioService.br();
            return;
        }

        std::string* priority = nullptr;
        if (command.hasOption("priority") && listItemService.priority().isNameValid(command.getOption("priority")))
        {
            priority = new std::string(command.getOption("priority"));
        }

        std::string* status = nullptr;
        if (command.hasOption("status") && listItemService.status().isNameValid(command.getOption("status")))
        {
            status = new std::string(command.getOption("status"));
        }

        time_t dueAt = 0;
        if (command.hasOption("deadline"))
        {
            try {
                std::string dateString = command.getOption("deadline");
                dueAt = DateHelpers::relativeDateToTimestamp(dateString);
            } catch (std::exception &e) {
                ioService.br();
                ioService.error("Invalid deadline date.");
                ioService.br();
                return;
            }
        }

        std::string id;
        try {
            id = listItemService.add(calculateValue(itemValueFromArguments), priority, status, dueAt);
        } catch (std::exception &e) {
            ioService.br();
            ioService.error("Item could not be added.");
            ioService.info(e.what());
            ioService.br();
            return;
        }

        free(priority);

        ioService.br();
        ioService.success("Item " + id + " correctly added.");
        ioService.br();
        return;
    }

    if (CommandService::isCommand(command, "edit"))
    {
        if (arguments.empty())
        {
            ioService.br();
            ioService.error("Please provide the ID and value for the list item.");
            ioService.br();
            return;
        }

        // Get the ID of the list item
        std::string id = itemValueFromArguments.at(0);

        if (!StringHelpers::isAlnum(id))
        {
            ioService.br();
            ioService.error("ID is not valid.");
            ioService.br();
            return;
        }

        itemValueFromArguments.erase(itemValueFromArguments.begin());

        try {
            std::string* priority = nullptr;
            if (command.hasOption("priority") && listItemService.priority().isNameValid(command.getOption("priority")))
            {
                priority = new std::string(command.getOption("priority"));
            }

            std::string* status = nullptr;
            if (command.hasOption("status") && listItemService.status().isNameValid(command.getOption("status")))
            {
                status = new std::string(command.getOption("status"));
            }

            time_t dueAt = 0;
            if (command.hasOption("deadline"))
            {
                try {
                    std::string dateString = command.getOption("deadline");
                    dueAt = DateHelpers::relativeDateToTimestamp(dateString);
                    listItemService.editDeadline(id, dueAt);
                } catch (std::exception &e) {
                    ioService.br();
                    ioService.error("Invalid deadline date.");
                    ioService.br();
                    return;
                }
            }

            std::string value = calculateValue(itemValueFromArguments);
            if (priority != nullptr)
            {
                listItemService.setPriority(id, priority);
                ioService.br();
                ioService.success("Priority of " + id + " updated.");
                ioService.br();
                return;
            }
            else if (status != nullptr)
            {
                listItemService.setStatus(id, status);
                ioService.br();
                ioService.success("Priority of " + id + " updated.");
                ioService.br();
                return;
            }

            listItemService.edit(id, value, priority, status);
        } catch (std::exception &e) {
            ioService.br();
            ioService.error("Item could not be updated.");
            ioService.br();
            return;
        }

        ioService.br();
        ioService.success("Item " + id + " updated.");
        ioService.br();
        return;
    }

    if (CommandService::isCommand(command, "append"))
    {
        if (arguments.size() < 2)
        {
            ioService.br();
            ioService.error("Please provide the ID and value for the list item.");
            ioService.br();
            return;
        }

        // Get the ID of the list item
        std::string id = itemValueFromArguments.at(0);

        if (!StringHelpers::isAlnum(id))
        {
            ioService.br();
            ioService.error("ID is not valid.");
            ioService.br();
            return;
        }

        itemValueFromArguments.erase(itemValueFromArguments.begin());

        try {
            listItemService.append(id, calculateValue(itemValueFromArguments));
            if (command.hasOption("priority") && listItemService.priority().isNameValid(command.getOption("priority")))
            {
                std::string priority = command.getOption("priority");
                listItemService.setPriority(id, &priority);
            }
            if (command.hasOption("status") && listItemService.status().isNameValid(command.getOption("status")))
            {
                std::string status = command.getOption("status");
                listItemService.setStatus(id, &status);
            }
        } catch (std::exception &e) {
            ioService.br();
            ioService.error("Item could not be updated.");
            ioService.br();
            return;
        }

        ioService.br();
        ioService.success("Item " + id + " updated.");
        ioService.br();
        return;
    }

    if (CommandService::isCommand(command, "prepend"))
    {
        if (arguments.size() < 2)
        {
            ioService.br();
            ioService.error("Please provide the ID and value for the list item.");
            ioService.br();
            return;
        }

        // Get the ID of the list item
        std::string id = itemValueFromArguments.at(0);

        if (!StringHelpers::isAlnum(id))
        {
            ioService.br();
            ioService.error("ID is not valid.");
            ioService.br();
            return;
        }

        itemValueFromArguments.erase(itemValueFromArguments.begin());

        try {
            listItemService.prepend(id, calculateValue(itemValueFromArguments));
            if (command.hasOption("priority") && listItemService.priority().isNameValid(command.getOption("priority")))
            {
                std::string priority = command.getOption("priority");
                listItemService.setPriority(id, &priority);
            }
            if (command.hasOption("status") && listItemService.status().isNameValid(command.getOption("status")))
            {
                std::string status = command.getOption("status");
                listItemService.setStatus(id, &status);
            }
        } catch (std::exception &e) {
            ioService.br();
            ioService.error("Item could not be updated.");
            ioService.br();
            return;
        }

        ioService.br();
        ioService.success("Item " + id + " updated.");
        ioService.br();
        return;
    }

    if (CommandService::isCommand(command, "deadline"))
    {
        if (arguments.size() < 2)
        {
            ioService.br();
            ioService.error("Please provide the ID of the item and the deadline date (expl: 2031.11.31).");
            ioService.br();
            return;
        }
        std::string stringDate = itemValueFromArguments.at(0);
        itemValueFromArguments.erase(itemValueFromArguments.begin());

        std::vector <std::string> ids = itemValueFromArguments;

        // clean the double ids
        ids.erase( unique( ids.begin(), ids.end() ), ids.end() );

        ioService.br();
        for (auto &id : ids)
        {
            if (!StringHelpers::isAlnum(id))
            {
                ioService.error("ID is not valid.");
                continue;
            }

            try {
                time_t dueAt = DateHelpers::relativeDateToTimestamp(stringDate);
                listItemService.editDeadline(id, dueAt);
                ioService.success("Deadline of " + id + " updated.");
            } catch (std::exception &e) {
                ioService.error(e.what());
                continue;
            }

        }
        ioService.br();
    }
}

std::string ListItemActions::calculateValue(std::vector<std::string> arguments)
{
    std::string value;
    for (int i = 0; i < arguments.size(); i++)
    {
        if (i == 0)
        {
            value += arguments.at(i);
            continue;
        }
        value += " " + arguments.at(i);
    }
    return value;
}


