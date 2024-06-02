#ifndef HELP_H
#define HELP_H

#include "../IOService/IOService.h"

class Help
{
public:
    explicit Help(IOService& ioService);
    void show();
    void commandNotFound();
    void commandOptionNotSupported();
    static std::string getVersion();
private:
    IOService& ioService;
    static std::vector <std::string> helpListForCli();
};

#endif //HELP_H
