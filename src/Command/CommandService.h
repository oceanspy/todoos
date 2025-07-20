#ifndef COMMANDSERVICE_H
#define COMMANDSERVICE_H

#include "Command.h"
#include "CommandList.h"
#include "CommandOption.h"
#include "CommandShortcut.h"

class CommandService
{
  public:
    explicit CommandService(CommandList& commandList, CommandOption& commandOption);
    std::vector<std::string> getBaseCommandList();
    std::string getMainCommandListAsString();
    bool isValid(std::string& commandName);
    static bool isCommand(Command& command, const std::string& commandName);
    bool isBeginningOfCommand(Command command);
    bool hasSubCommand(Command command);
    static std::string getCommandName(const std::string& shortcut);
    Command getSubCommand(Command command);
    CommandList& list();
    CommandOption& options();
    static bool isCommandValidWithOption(std::string commandName, std::map<std::string, std::string> options);

  private:
    CommandList& commandList;
    CommandOption& commandOption;
};

#endif // COMMANDSERVICE_H
