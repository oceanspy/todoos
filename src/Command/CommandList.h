#ifndef TODOOS_COMMANDLIST_H
#define TODOOS_COMMANDLIST_H

#include <string>
#include <map>
#include "CommandOption.h"
#include <vector>

class CommandList {
public:
    CommandList();
    std::vector <std::string> getMainCommandNames(const bool showOnlyAutocomplete = true);
    int getCommandId(const std::string &command);
    bool isValid(const std::string& commandNameToEvaluate);
    bool isBeginningOfCommand(const std::string& partialCommandNameToEvaluate);
    static bool isCommandValidWithOptions(std::string option, std::map<std::string, std::string> options);

    enum CommandIds {
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
        TO_DO = 31,
        START = 32,
        PAUSE = 33,
        REVIEW = 34,
        PENDING = 35,
        COMPLETE = 36,
        CANCEL = 37,
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
        RESET = 90,
        COMMANDS = 100
    };

private:
    std::vector <int> commandIds;

    struct CommandEntity {
        int id;
        std::string name;
        std::vector <int> childrenIds;
        bool showInAutocomplete = true;
    };

    std::map <int, CommandEntity> commands;
    void make();
};


#endif //TODOOS_COMMANDLIST_H
