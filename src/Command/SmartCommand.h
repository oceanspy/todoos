#ifndef SMARTCOMMAND_H
#define SMARTCOMMAND_H

#include "../List/ListService.h"
#include "Command.h"
#include "CommandService.h"

class SmartCommand
{
  public:
    explicit SmartCommand();
    Command apply(Command& command);
};

#endif // SMARTCOMMAND_H
