#ifndef LISTREPOSITORY_H
#define LISTREPOSITORY_H

#include "../Config/ConfigService.h"
#include "../IOService/IOService.h"
#include "../List/ListEntity.h"
#include "../Serializers/DataSerializerInterface.h"
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

class ListRepository
{
  public:
    ListRepository();
    std::vector<ListEntity> get();
    ListEntity find(const std::string& key);
    void create(const ListEntity& item);
    bool update(const std::string& listName, const ListEntity& item);
    void remove(std::string& name);
};

#endif // LISTREPOSITORY_H
