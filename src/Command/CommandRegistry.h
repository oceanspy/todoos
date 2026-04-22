#ifndef TODOOS_COMMANDREGISTRY_H
#define TODOOS_COMMANDREGISTRY_H

#include "CommandOption.h"
#include <map>
#include <string>
#include <vector>

class CommandRegistry
{
  public:
    CommandRegistry();
    std::vector<std::string> getMainCommandNames(const bool showOnlyAutocomplete = true);
    int getCommandId(const std::string& command);
    bool isValid(const std::string& commandNameToEvaluate);
    bool isBeginningOfCommand(const std::string& partialCommandNameToEvaluate);
    static bool isCommandValidWithOptions(std::string option, std::map<std::string, std::string> options);

    enum CommandIds
    {
        SHOW = 0,
        ADD = 1,
        EDIT = 2,
        APPEND = 3,
        PREPEND = 4,
        FIND = 10,
        PRIORITY = 20,
        INCREASE = 21,
        DECREASE = 22,
        STATUS = 30,
        QUEUE = 31,
        START = 32,
        PAUSE = 33,
        TRIAGE = 34,
        BLOCKED = 35,
        COMPLETE = 36,
        CANCEL = 37,
        RESET = 38,
        REMOVE = 40,
        ARCHIVE = 41,
        RESTORE = 42,
        MOVE = 50,
        COPY = 51,
        EMPTY = 52,
        CLEAN = 53,
        DUPLICATE = 54,
        DEADLINE = 60,
        LIST = 70,
        USE = 71,
        STATS = 80,
        DESCRIBE = 90,
        COMMANDS = 100
    };

  private:
    std::vector<int> commandIds;

    struct CommandEntity
    {
        int id;
        std::string name;
        std::vector<int> childrenIds;
        bool showInAutocomplete = true;
    };

    std::map<int, CommandEntity> commands;
    void make();
};

#endif // TODOOS_COMMANDREGISTRY_H
