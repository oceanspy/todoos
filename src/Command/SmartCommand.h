#ifndef SMARTCOMMAND_H
#define SMARTCOMMAND_H

#include "Command.h"
#include "CommandService.h"

class SmartCommand
{
  public:
    explicit SmartCommand(const Command& command);
    Command apply();

  private:
    Command command;
};

#endif // SMARTCOMMAND_H
