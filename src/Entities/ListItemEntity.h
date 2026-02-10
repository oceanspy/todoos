#ifndef LISTITEMENTITY_H
#define LISTITEMENTITY_H

#include "../List/ListItems/PriorityService.h"
#include "../List/ListItems/StatusService.h"
#include "../List/ListName.h"
#include <string>

class ListItemEntity
{
  public:
    ListItemEntity(ListName& listName);
    [[nodiscard]] const std::string* getId() const;
    [[nodiscard]] const int* getPosition() const;
    [[nodiscard]] const ListName* getListName() const;
    [[nodiscard]] const std::string* getListVariant() const;
    [[nodiscard]] const std::string* getValue() const;
    [[nodiscard]] const PriorityEntity* priority() const;
    [[nodiscard]] const StatusEntity* status() const;
    [[nodiscard]] const time_t* getDueAt() const;
    [[nodiscard]] const time_t* getClosedAt() const;
    [[nodiscard]] const time_t* getCreatedAt() const;
    [[nodiscard]] const time_t* getUpdatedAt() const;
    void setId(std::string idStr);
    void setPosition(int positionInt);
    void setValue(const std::string& valueStr);
    void setPriority(const PriorityEntity& priorityEntity);
    void setStatus(const StatusEntity& statusEntity);
    void setDueAt(time_t dueAtDate);
    void setClosedAt(time_t closedAtDate);
    void setCreatedAt(time_t createdAt);
    void setUpdatedAt(time_t updatedAtDate);
    static ListItemEntity set(const std::string& id,
                              const std::string& value,
                              const PriorityEntity& priority,
                              const StatusEntity& status,
                              const time_t& dueAt,
                              const time_t& closedAt,
                              const time_t& createdAt,
                              const time_t& updatedAt,
                              ListName& listName);
    static ListItemEntity setFromVector(PriorityService& priorityService,
                                        StatusService& statusService,
                                        const std::vector<std::string>& item,
                                        ListName& listName);
    static std::vector<std::string> makeVector(const ListItemEntity& listItemEntity);

  private:
    std::string id;
    int position = 0;
    ListName listName;
    std::string value;
    PriorityEntity priorityEntity;
    StatusEntity statusEntity;
    time_t dueAt = 0;
    time_t closedAt = 0;
    time_t createdAt = 0;
    time_t updatedAt = 0;
};

#endif // LISTITEMENTITY_H
