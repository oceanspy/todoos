#ifndef PRIORITY_H
#define PRIORITY_H

#include "../../List/ListItemService.h"
#include "../../IOService/IOService.h"
#include "../../Command/Command.h"

class Priority {
public:
    Priority(IOService& ioService, Command& command, ListItemService& listItemService);
    void increase();
    void decrease();
    void set();
private:
    IOService& ioService;
    Command& command;
    ListItemService& listItemService;
};



#endif //PRIORITY_H
