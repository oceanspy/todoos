#include "CommandShortcut.h"

std::map <std::string, std::string> CommandShortcut::getShortcutCommands()
{
    std::map <std::string, std::string> shortCutCommands;
    shortCutCommands["l"] = "list";
    shortCutCommands["a"] = "add";
    shortCutCommands["rm"] = "remove";
    shortCutCommands["e"] = "edit";
    shortCutCommands["rename"] = "edit";
    shortCutCommands["inc"] = "increase";
    shortCutCommands["dec"] = "decrease";
    shortCutCommands["todo"] = "to-do";
    shortCutCommands["to-do"] = "to-do";
    shortCutCommands["resume"] = "to-do";
    shortCutCommands["start"] = "start";
    shortCutCommands["pause"] = "pause";
    shortCutCommands["review"] = "review";
    shortCutCommands["pending"] = "pend";
    shortCutCommands["complete"] = "complete";
    shortCutCommands["done"] = "complete";
    shortCutCommands["dln"] = "deadline";
    shortCutCommands["reset"] = "reset";
    shortCutCommands["cancel"] = "cancel";
    shortCutCommands["settings"] = "config";
    shortCutCommands["options"] = "config";
    shortCutCommands["p"] = "priority";
    shortCutCommands["append"] = "append";
    shortCutCommands["prepend"] = "prepend";
    shortCutCommands["empty"] = "empty";
    shortCutCommands["search"] = "find";
    shortCutCommands["s"] = "find";
    shortCutCommands["f"] = "find";
    shortCutCommands["move"] = "move-to";
    shortCutCommands["mv"] = "move-to";
    shortCutCommands["rename"] = "move-to";
    shortCutCommands["copy"] = "copy-to";
    shortCutCommands["cp"] = "copy-to";
    shortCutCommands["current"] = "current";

    return shortCutCommands;
}

std::vector <std::string> CommandShortcut::getShortcutsForCommand(const std::string& commandName)
{
    std::map <std::string, std::string> shortcutCommands = getShortcutCommands();

    std::vector <std::string> command;
    for (auto const& [key, val] : shortcutCommands)
    {
        if (val == commandName)
        {
            command.push_back(key);
        }
    }

    return command;
}

std::string CommandShortcut::getCommandName(const std::string& shortcut)
{
    std::map <std::string, std::string> shortCutCommands = getShortcutCommands();

    for (auto const& [key, val] : shortCutCommands)
    {
        if (key == shortcut)
        {
            return val;
        }
    }

    return shortcut;
}

bool CommandShortcut::isCommand(Command command, const std::string& commandName)
{
    if (command.getName() == commandName)
    {
        return true;
    }

    std::map <std::string, std::string> shortCutCommands = getShortcutCommands();
    for (auto const& [key, val] : shortCutCommands)
    {
        if (val == commandName && command.getName() == key)
        {
            return true;
        }
    }

    return false;
}
