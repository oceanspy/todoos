#ifndef LISTITEMENTITY_H
#define LISTITEMENTITY_H

#include "../List/ListItems/PriorityService.h"
#include "../List/ListItems/StatusService.h"
#include <string>

class ListItemEntity
{
  public:
    [[nodiscard]] const std::string* getId() const;
    [[nodiscard]] const int* getPosition() const;
    [[nodiscard]] const std::string* getListName() const;
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
    void setListName(std::string listNameStr);
    void setListVariant(std::string listVariantStr);
    void setValue(const std::string& valueStr);
    void setPriority(const PriorityEntity& priorityEntity);
    void setStatus(const StatusEntity& statusEntity);
    void setDueAt(time_t dueAtDate);
    void setClosedAt(time_t closedAtDate);
    void setCreatedAt(time_t createdAt);
    void setUpdatedAt(time_t updatedAtDate);
    static ListItemEntity set(const std::string& id,
                              const std::string& listName,
                              const std::string& value,
                              const PriorityEntity& priority,
                              const StatusEntity& status,
                              const time_t& dueAt,
                              const time_t& closedAt,
                              const time_t& createdAt,
                              const time_t& updatedAt);
    static ListItemEntity setFromVector(PriorityService& priorityService,
                                        StatusService& statusService,
                                        const std::vector<std::string>& item,
                                        const std::string& listName,
                                        const std::string& listVariant);
    static std::vector<std::string> makeVector(const ListItemEntity& listItemEntity);

  private:
    std::string id;
    int position = 0;
    std::string listName;
    std::string listVariant;
    std::string value;
    PriorityEntity priorityEntity;
    StatusEntity statusEntity;
    time_t dueAt = 0;
    time_t closedAt = 0;
    time_t createdAt = 0;
    time_t updatedAt = 0;
};

#endif // LISTITEMENTITY_H
