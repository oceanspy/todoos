#ifndef COMMANDSERVICE_H
#define COMMANDSERVICE_H

#include "Command.h"
#include "CommandRegistry.h"
#include "CommandOption.h"
#include "CommandShortcut.h"

class CommandService
{
  public:
    explicit CommandService(CommandRegistry& commandList, CommandOption& commandOption);
    std::vector<std::string> getBaseCommandList();
    std::string getMainCommandListAsString();
    bool isValid(Command& command);
    static bool isCommand(Command& command, const std::string& commandName);
    bool isBeginningOfCommand(Command command);
    bool hasSubCommand(Command command);
    static std::string getCommandName(const std::string& shortcut);
    Command getSubCommand(Command command);
    CommandRegistry& list();
    CommandOption& options();
    static bool isCommandValidWithOption(Command& command);

  private:
    CommandRegistry& commandList;
    CommandOption& commandOption;
};

#endif // COMMANDSERVICE_H
