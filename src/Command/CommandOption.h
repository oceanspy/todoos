#ifndef TODOOS_COMMANDOPTION_H
#define TODOOS_COMMANDOPTION_H

#include <string>
#include <map>
#include "CommandOption.h"
#include <string>

class CommandOption {
public:
    CommandOption();
    bool isValidOption(const std::string& arg);
    bool isOptionWithValue(const std::string& arg);
    std::string shortToLongOption(const std::string& shortOption);
    std::string getLongOptionName(const std::string& arg);
private:
    void populateOptions();
    std::map <std::string, std::string> options;
    bool isShortOption(const std::string& arg);
};


#endif //TODOOS_COMMANDOPTION_H
