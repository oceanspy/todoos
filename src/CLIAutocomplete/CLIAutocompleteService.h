#ifndef CLIAUTOCOMPLETESERVICE_H
#define CLIAUTOCOMPLETESERVICE_H
#include "../IOService/IOService.h"
#include "../Command/Command.h"
#include "../Command/CommandService.h"
#include "../List/ListService.h"
#include "../List/ListItemService.h"
#include "../Helpers/DateHelpers.h"
#include "../Helpers/StringHelpers.h"


class CLIAutocompleteService {
public:
    CLIAutocompleteService(IOService& ioService,
                           CommandService& commandService,
                           Command& command,
                           ListService& listService,
                           ListItemService& listItemService
                           );
    bool getCompletion();
protected:
    void autocompleteOptionList();
    void autocompleteBase();
    void autocompletePriority(const Command& firstSubCommand);
    void autocompleteStatus(const Command& firstSubCommand);
    void autocompleteList(const Command& firstSubCommand);
    void autocompleteUseList(const Command& firstSubCommand);
    void autocompleteMoveList(const Command& firstSubCommand);
    bool isValidListItemId(std::string id);
    void getAllLists(std::string& listString);
    void showListItemId(const std::vector <std::string>& variants = {"default"});
    void autocompleteId(const Command& firstSubCommand, const std::vector <std::string>& variants = {"default"});
    void autocompleteIdIndefinitely(const Command& firstSubCommand, const std::vector <std::string>& variants = {"default"});
    std::string getDeadline();
private:
    IOService& ioService;
    CommandService& commandService;
    Command& command;
    ListService& listService;
    ListItemService& listItemService;
    bool isStartOfCommand(std::string listOfCommandNames, std::string partialCommandName);
};



#endif //CLIAUTOCOMPLETESERVICE_H
