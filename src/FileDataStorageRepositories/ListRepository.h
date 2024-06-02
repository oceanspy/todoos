#ifndef LISTREPOSITORY_H
#define LISTREPOSITORY_H

#include <filesystem>
#include <string>
#include <vector>
#include <fstream>
#include "../Config/ConfigService.h"
#include "../FileDataStorage/FileDataServiceInterface.h"
#include "../IOService/IOService.h"
#include "../Entities/ListEntity.h"

class ListRepository {
public:
    ListRepository(ConfigService& configService, FileDataServiceInterface *fileDataService);
    std::vector <ListEntity> get();
    ListEntity find(const std::string& key);
    void create(const ListEntity& item);
    bool update(const std::string& listName, const ListEntity& item);
    void remove(std::string& name);
    void resetCache();
private:
    ConfigService& configService;
    FileDataServiceInterface* fileDataService{};
    std::string listName;
    std::filesystem::path path;
    std::vector <ListEntity> cacheItems = {};
};

#endif //LISTREPOSITORY_H
