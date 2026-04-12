#ifndef LISTREPOSITORY_H
#define LISTREPOSITORY_H

#include "../Config/ConfigService.h"
#include "../Serializers/DataSerializerInterface.h"
#include "../IOService/IOService.h"
#include "../List/ListEntity.h"
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

class ListRepository
{
  public:
    ListRepository(ConfigService& configService, DataSerializerInterface* fileDataService);
    std::vector<ListEntity> get();
    ListEntity find(const std::string& key);
    void create(const ListEntity& item);
    bool update(const std::string& listName, const ListEntity& item);
    void remove(std::string& name);
    void resetCache();

  private:
    ConfigService& configService;
    DataSerializerInterface* fileDataService{};
    std::string listName;
    std::filesystem::path path;
    std::vector<ListEntity> cacheItems = {};
};

#endif // LISTREPOSITORY_H
