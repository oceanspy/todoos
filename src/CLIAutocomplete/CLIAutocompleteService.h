#ifndef CLIAUTOCOMPLETESERVICE_H
#define CLIAUTOCOMPLETESERVICE_H
#include "../Command/Command.h"
#include "../Command/CommandService.h"
#include "../Helpers/DateHelpers.h"
#include "../Helpers/StringHelpers.h"
#include "../IOService/IOService.h"
#include "../List/ListItemService.h"
#include "../List/ListService.h"

class CLIAutocompleteService
{
  public:
    CLIAutocompleteService(IOService& ioService,
                           CommandService& commandService,
                           Command& command,
                           ListService& listService,
                           ListItemService& listItemService);
    bool getCompletion();

  protected:
    void autocompleteOptionList();
    void autocompleteBase();
    void autocompletePriority(const Command& firstSubCommand, std::vector<ListName>& listNames);
    void autocompleteStatus(const Command& firstSubCommand, std::vector<ListName>& listNames);
    void autocompleteListActions(const Command& firstSubCommand);
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
    Command& command;
    ListService& listService;
    ListItemService& listItemService;
    bool isStartOfCommand(std::string listOfCommandNames, std::string partialCommandName);
};

#endif // CLIAUTOCOMPLETESERVICE_H
