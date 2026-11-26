#ifndef TODOOS_COMMANDOPTION_H
#define TODOOS_COMMANDOPTION_H

#include <map>
#include <string>

class CommandOption
{
  public:
    CommandOption();
    bool isValidOption(const std::string& arg);
    // a short option name, as given after "-": "l"
    bool isShortOption(const std::string& arg);
    // a long option name, as given after "--": "list"
    bool isLongOption(const std::string& arg);
    bool isOptionWithValue(const std::string& arg);
    std::string shortToLongOption(const std::string& shortOption);
    std::string getLongOptionName(const std::string& arg);
    // All option names with their dashes, space separated: "-l -p ..." or "--list --priority ..."
    std::string getShortNamesAsString();
    std::string getLongNamesAsString();

  private:
    void populateOptions();
    std::map<std::string, std::string> options;
};

#endif // TODOOS_COMMANDOPTION_H
