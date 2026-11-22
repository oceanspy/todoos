#ifndef LISTITEMENTITY_H
#define LISTITEMENTITY_H

#include "../../List/ListName.h"
#include "PriorityService.h"
#include "StatusService.h"
#include <string>

class ListItemEntity
{
  public:
    ListItemEntity(ListName& listName);
    [[nodiscard]] const std::string* getId() const;
    // Id of the parent item in the same list (and variant); empty for a top-level item
    [[nodiscard]] const std::string* getParentId() const;
    [[nodiscard]] bool hasParent() const;
    [[nodiscard]] const int* getPosition() const;
    [[nodiscard]] const ListName* getListName() const;
    [[nodiscard]] const std::string* getListVariant() const;
    [[nodiscard]] const std::string* getValue() const;
    [[nodiscard]] const PriorityEntity* priority() const;
    [[nodiscard]] const StatusEntity* status() const;
    [[nodiscard]] const bool* hasDescription() const;
    [[nodiscard]] const time_t* getDueAt() const;
    [[nodiscard]] const time_t* getClosedAt() const;
    [[nodiscard]] const time_t* getCreatedAt() const;
    [[nodiscard]] const time_t* getUpdatedAt() const;
    void setId(std::string idStr);
    void setParentId(const std::string& parentIdStr);
    void setPosition(int positionInt);
    void setValue(const std::string& valueStr);
    void setPriority(const PriorityEntity& priorityEntity);
    void setStatus(const StatusEntity& statusEntity);
    void setHasDescription(const bool& description);
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
    // Number of columns of a stored row: id, parent_id, value, priority, status, dueAt, closedAt,
    // createdAt, updatedAt
    static constexpr size_t ROW_SIZE = 9;
    static std::vector<std::string> makeVector(const ListItemEntity& listItemEntity);

  private:
    std::string id;
    std::string parentId;
    int position = 0;
    bool descriptionExists = false;
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
