#ifndef STATUS_H
#define STATUS_H

#include "../../List/ListItemService.h"
#include "../../IOService/IOService.h"
#include "../../Command/Command.h"

class Status {
public:
    Status(IOService& ioService, Command& command, ListItemService& listItemService);
    void markAs(int status);
    void set();
    void reset();
private:
    IOService& ioService;
    Command& command;
    ListItemService& listItemService;
};



#endif //STATUS_H
