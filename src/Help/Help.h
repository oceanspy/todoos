#ifndef HELP_H
#define HELP_H

#include "../Command/Command.h"
#include "../IOService/IOService.h"

class Help
{
  public:
    explicit Help(IOService& ioService);
    void show();
    void commandNotFound();
    void commandNotFoundSkipCommandAutocomplete(Command& command);
    void commandOptionNotSupported();
    void commandOptionNotSupportedSkipCommandAutocomplete(Command& command);
    void listNotFound(std::string list);
    static std::string getVersion();

  private:
    IOService& ioService;
    static std::vector<std::string> helpListForCli();
};

#endif // HELP_H
