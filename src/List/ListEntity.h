#ifndef LISTENTITY_H
#define LISTENTITY_H

#include <string>
#include <vector>

class ListEntity
{
  public:
    [[nodiscard]] const std::string* getName() const;
    [[nodiscard]] const std::string* getType() const;
    [[nodiscard]] const std::string* getSorting() const;
    [[nodiscard]] const bool* isHidden() const;
    void setName(const std::string name);
    void setType(const std::string type);
    void setSorting(const std::string sorting);
    void setHidden(const bool hidden);
    static ListEntity setFromVector(const std::vector<std::string>& item);
    static std::vector<std::string> makeVector(const ListEntity& listEntity);

  private:
    std::string name;
    std::string theme;
    std::string sorting;
    bool hidden;
};

#endif // LISTENTITY_H
