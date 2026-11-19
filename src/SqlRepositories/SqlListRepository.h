#ifndef SQLLISTREPOSITORY_H
#define SQLLISTREPOSITORY_H

#include "../List/ListEntity.h"
#include "../List/ListRepositoryInterface.h"
#include <string>
#include <vector>

class SqlListRepository : public ListRepositoryInterface
{
  public:
    SqlListRepository();
    std::vector<ListEntity> get() override;
    ListEntity find(const std::string& name) override;
    void create(const ListEntity& item) override;
    bool update(const std::string& name, const ListEntity& item) override;
    void rename(const std::string& oldName, const std::string& newName) override;
    void remove(const std::string& name) override;
    void removeKeepingBackup(const std::string& name) override;
};

#endif // SQLLISTREPOSITORY_H
