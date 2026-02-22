#ifndef LIST_H
#define LIST_H

#include "../Config/ConfigService.h"
#include "../Events/EventBus.h"
#include "../Exception/ListNotFoundException.h"
#include "../FileDataStorageRepositories/ListRepository.h"
#include "../IOService/IOService.h"
#include "ListName.h"
#include <regex>
#include <string>
#include <utility>

class ListService
{
  public:
    ListService(IOService& ioService, ConfigService& configService, ListRepository& listRepository, EventBus& bus);
    ListName createUsedListName();
    ListName createListName(std::string name, std::string variant = "default");
    std::vector<ListName> getAutocompletedLists(std::string& name, std::string& variant);
    ListEntity find(const std::string& listName);
    void add(const std::string& listName, const std::string& type = "default", const std::string& sorting = "default");
    void edit(const std::string& oldListName, std::string newListName);
    void remove(std::string& listName);
    bool isListExist(const std::string& name);
    bool use(const std::string& listName);
    std::string getType(const std::string& listName);
    std::vector<ListEntity> get(bool keepHidden = false);
    void subscribeToEvents(EventBus& eventBus);
    static void validateListName(const std::string& newListName);

  private:
    IOService& ioService;
    ConfigService& configService;
    ListRepository& listRepository;
    EventBus& bus;
    std::string list;
    static std::vector<ListEntity> sort(std::vector<ListEntity> listItems);
};

#endif // LIST_H
