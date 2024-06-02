#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include <vector>
#include <map>

class Command {
public:
    Command(std::string command, std::vector <std::string> arguments, std::map <std::string, std::string> options, std::string rawCommand = "");
    std::string getName();
    std::vector <std::string> getArguments();
    std::map <std::string, std::string> getOptions();
    std::string getOption(const std::string& optionName);
    bool hasOption(const std::string& optionName);
    std::string getRawCommand();
    std::string getLastCommandInput();
private:
    std::string command;
    std::vector <std::string> arguments;
    std::map <std::string, std::string> options = {};
    std::string rawCommand;
};

#endif //COMMAND_H
