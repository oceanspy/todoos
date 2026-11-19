#ifndef LISTITEMREPOSITORY_H
#define LISTITEMREPOSITORY_H

#include "../Config/ConfigService.h"
#include "../Exception/ListItemNotFoundException.h"
#include "../Helpers/StringHelpers.h"
#include "../IOService/IOService.h"
#include "../List/ListItemRepositoryInterface.h"
#include "../List/ListItems/ListItemEntity.h"
#include "../List/ListItems/PriorityService.h"
#include "../List/ListItems/StatusService.h"
#include "../List/ListName.h"
#include "../Serializers/DataSerializerInterface.h"
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class ListItemRepository : public ListItemRepositoryInterface
{
  public:
    explicit ListItemRepository(ConfigService& configService,
                                DataSerializerInterface* fileDataService,
                                PriorityService& priorityService,
                                StatusService& statusService);
    std::vector<ListItemEntity> get(ListName& listName) override;
    ListItemEntity find(const std::string& id, ListName& listName) override;
    void create(const ListItemEntity& item, ListName& listName) override;
    bool update(const std::string& id, ListName& listName, const ListItemEntity& item) override;
    bool remove(const std::string& id, ListName& listName) override;
    void resetCache(const std::string& filePath = "");

  private:
    ConfigService& configService;
    DataSerializerInterface* fileDataService{};
    PriorityService& priorityService;
    StatusService& statusService;
    std::unordered_map<std::string, std::vector<ListItemEntity>> cacheItems;
    std::string getFilePath(ListName& listName);
};

#endif // LISTITEMREPOSITORY_H
