#ifndef COMMANDCONTROLLER_H
#define COMMANDCONTROLLER_H

#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include "CommandOption.h"

class CommandValidation
{
public:
    CommandValidation(CommandOption& commandOption, int argc, const char* argv[]);
    bool isCommandEmpty();
    std::string getCommandName();
    std::vector <std::string> getCommandArguments();
    std::map <std::string, std::string> getCommandOptions();
    std::string getRawCommand();
    bool make();
private:

protected:
    CommandOption& commandOption;
    const int argc;
    std::string rawCommand;
    std::string commandName;
    std::vector <std::string> arguments;
    std::map<std::string, std::string> options;
    const char** argv;

    static std::string removeLeadingDash(std::string basicString);
};

#endif //COMMANDCONTROLLER_H
