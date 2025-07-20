#ifndef COMMANDSHORTCUTSERVICE_H
#define COMMANDSHORTCUTSERVICE_H

#include <map>
#include <string>
#include <vector>

#include "Command.h"

class CommandShortcut
{
  public:
    static std::map<std::string, std::string> getShortcutCommands();
    static std::vector<std::string> getShortcutsForCommand(const std::string& commandName);
    static std::string getCommandName(const std::string& shortcut);
    static bool isCommand(Command command, const std::string& commandName);
};

#endif // COMMANDSHORTCUTSERVICE_H
