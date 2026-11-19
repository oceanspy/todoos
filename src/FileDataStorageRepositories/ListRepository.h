#ifndef LISTREPOSITORY_H
#define LISTREPOSITORY_H

#include "../Config/ConfigService.h"
#include "../FileStorage/FileStorageService.h"
#include "../List/ListEntity.h"
#include "../List/ListRepositoryInterface.h"
#include "../Serializers/DataSerializerInterface.h"
#include <filesystem>
#include <string>
#include <vector>

// File-based list repository: the list of lists is stored in a serialized file,
// and each list's items live in their own files, managed through FileStorageService.
class ListRepository : public ListRepositoryInterface
{
  public:
    ListRepository(ConfigService& configService, DataSerializerInterface* fileDataService);
    std::vector<ListEntity> get() override;
    ListEntity find(const std::string& name) override;
    void create(const ListEntity& item) override;
    bool update(const std::string& name, const ListEntity& item) override;
    void rename(const std::string& oldName, const std::string& newName) override;
    void remove(const std::string& name) override;
    void removeKeepingBackup(const std::string& name) override;
    void resetCache();

    static const std::string BACKUP_PREFIX;

  private:
    ConfigService& configService;
    DataSerializerInterface* fileDataService{};
    FileStorageService fileStorageService;
    std::filesystem::path path;
    std::vector<ListEntity> cacheItems = {};
    void removeEntry(const std::string& name);
};

#endif // LISTREPOSITORY_H
