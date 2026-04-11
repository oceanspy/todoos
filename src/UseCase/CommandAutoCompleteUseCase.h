#ifndef COMMANDAUTOCOMPLETESERVICE_H
#define COMMANDAUTOCOMPLETESERVICE_H
#include "../Command/Command.h"
#include "../Command/CommandService.h"
#include "../Helpers/DateHelpers.h"
#include "../Helpers/StringHelpers.h"
#include "../IOService/IOService.h"
#include "../List/ListItemService.h"
#include "../List/ListService.h"

class CommandAutoCompleteUseCase
{
  public:
    CommandAutoCompleteUseCase(IOService& ioService,
                               CommandService& commandService,
                               ListService& listService,
                               ListItemService& listItemService);
    bool execute(Command& command);

  protected:
    void autocompleteOptionList(Command& command);
    void autocompleteBase();
    void autocompletePriority(const Command& firstSubCommand, std::vector<ListName>& listNames);
    void autocompleteStatus(const Command& firstSubCommand, std::vector<ListName>& listNames);
    void autocompleteListActions(const Command& firstSubCommand, Command& command);
    void autocompleteListIndefinitely(const Command& firstSubCommand);
    void autocompleteUseList(const Command& firstSubCommand);
    void autocompleteMoveList(const Command& firstSubCommand, std::vector<ListName>& listNames);
    bool isValidListItemId(std::string id, ListName& listName);
    void getAllLists(std::string& listString);
    void showListItemId(std::vector<ListName>& listNames);
    void autocompleteId(const Command& firstSubCommand, std::vector<ListName>& listNames);
    void autocompleteIdIndefinitely(const Command& firstSubCommand, std::vector<ListName>& listNames);
    std::string getDeadline();

  private:
    IOService& ioService;
    CommandService& commandService;
    ListService& listService;
    ListItemService& listItemService;
    bool isStartOfCommand(std::string listOfCommandNames, std::string partialCommandName);
};

#endif // COMMANDAUTOCOMPLETESERVICE_H
