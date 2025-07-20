#ifndef LISTITEMREPOSITORY_H
#define LISTITEMREPOSITORY_H

#include "../Config/ConfigService.h"
#include "../Entities/ListItemEntity.h"
#include "../FileDataStorage/FileDataServiceInterface.h"
#include "../Helpers/StringHelpers.h"
#include "../IOService/IOService.h"
#include "../List/ListItems/PriorityService.h"
#include "../List/ListItems/StatusService.h"
#include <filesystem>
#include <string>
#include <vector>

class ListItemRepository
{
  public:
    explicit ListItemRepository(ConfigService& configService,
                                FileDataServiceInterface* fileDataService,
                                PriorityService& priorityService,
                                StatusService& statusService);
    ListItemRepository& load(const std::string& listNameStr, std::string variantStr = "default");
    ListItemRepository& loadVariant(const std::string& variantStr = "default");
    std::vector<ListItemEntity> get();
    ListItemEntity find(const std::string& id);
    void create(const ListItemEntity& item);
    bool update(const std::string& id, const ListItemEntity& item);
    bool remove(const std::string& id);
    void resetCache();

  private:
    ConfigService& configService;
    FileDataServiceInterface* fileDataService{};
    PriorityService& priorityService;
    StatusService& statusService;
    std::string listName;
    std::string listVariant;
    std::filesystem::path listFilePath;
    std::vector<ListItemEntity> cacheItems = {};
};

#endif // LISTITEMREPOSITORY_H
