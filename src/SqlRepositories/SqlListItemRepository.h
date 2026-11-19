#ifndef SQLLISTITEMREPOSITORY_H
#define SQLLISTITEMREPOSITORY_H

#include "../Exception/ListItemNotFoundException.h"
#include "../List/ListItemRepositoryInterface.h"
#include "../List/ListItems/ListItemEntity.h"
#include "../List/ListItems/PriorityService.h"
#include "../List/ListItems/StatusService.h"
#include "../List/ListName.h"
#include <string>
#include <vector>

class SqlListItemRepository : public ListItemRepositoryInterface
{
  public:
    explicit SqlListItemRepository(PriorityService& priorityService, StatusService& statusService);
    std::vector<ListItemEntity> get(ListName& listName) override;
    ListItemEntity find(const std::string& id, ListName& listName) override;
    void create(const ListItemEntity& item, ListName& listName) override;
    bool update(const std::string& id, ListName& listName, const ListItemEntity& item) override;
    bool remove(const std::string& id, ListName& listName) override;

  private:
    PriorityService& priorityService;
    StatusService& statusService;
};

#endif // SQLLISTITEMREPOSITORY_H
