#ifndef LISTENTITY_H
#define LISTENTITY_H

#include <string>
#include <vector>

class ListEntity {
public:
    std::string getName() const;
    std::string getType() const;
    std::string getSorting() const;
    bool isHidden() const;
    void setName(std::string name);
    void setType(std::string type);
    void setSorting(std::string sorting);
    void setHidden(bool hidden);
    static ListEntity setFromVector(std::vector <std::string> item);
    static std::vector <std::string> makeVector(const ListEntity& listEntity);
private:
    std::string name;
    std::string theme;
    std::string sorting;
    bool hidden;
};

#endif //LISTENTITY_H
