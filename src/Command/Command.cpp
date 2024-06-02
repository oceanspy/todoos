#include "Command.h"

#include <string>
#include <vector>
#include <algorithm>

Command::Command(std::string command, std::vector <std::string> arguments, std::map <std::string, std::string> options, std::string rawCommand)
    : command(command), arguments(arguments), options(options), rawCommand(rawCommand)
{

}

std::string Command::getName()
{
    return command;
}

std::vector <std::string> Command::getArguments()
{
    return arguments;
}

std::map <std::string, std::string> Command::getOptions()
{
    return options;
}

std::string Command::getRawCommand()
{
    return rawCommand;
}

std::string Command::getLastCommandInput()
{
    return rawCommand.substr(rawCommand.find_last_of(" ") + 1);
}

std::string Command::getOption(const std::string& optionName)
{
    for (const auto& pair : options) {
        if (pair.first == optionName) {
            return pair.second;
        }
    }
    return "";
}

bool Command::hasOption(const std::string& optionName) {
    return std::ranges::any_of(options, [&optionName](const auto& option) {
        if (option.first == optionName) {
            return true;
        }

        return false;
    });
}
