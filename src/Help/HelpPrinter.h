#ifndef HELP_H
#define HELP_H

#include "../Command/Command.h"
#include "../IOService/IOService.h"
#include "../List/ListName.h"

class HelpPrinter
{
  public:
    explicit HelpPrinter(IOService& ioService);
    void show();
    void commandNotFound();
    void commandNotFoundSkipCommandAutocomplete(Command& command);
    void commandOptionNotSupported();
    void commandOptionNotSupportedSkipCommandAutocomplete(Command& command);
    void listNotFound(const std::string& list);
    void listItemNotFound(const std::string& id, const ListName& listName);
    static std::string getVersion();

  private:
    IOService& ioService;
    static std::vector<std::string> helpListForCli();
};

#endif // HELP_H
