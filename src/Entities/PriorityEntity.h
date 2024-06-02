#ifndef TODOOS_PRIORITYENTITY_H
#define TODOOS_PRIORITYENTITY_H

#include <string>
#include <vector>
#include "../Helpers/BashStyle.h"

class PriorityEntity {
public:
    [[nodiscard]] int getId() const;
    [[nodiscard]] int getPosition() const;
    [[nodiscard]] std::string getName() const;
    [[nodiscard]] std::string getIcon() const;
    [[nodiscard]] std::string getColor() const;
    void setId(int id);
    void setPosition(int position);
    void setName(std::string name);
    void setIcon(std::string icon);
    void setColor(std::string color);
    static PriorityEntity setFromName(std::string priorityName);
    static PriorityEntity setFromVector(std::vector <std::string> item);
    static std::vector <std::string> makeVector(const PriorityEntity& priorityEntity);
private:
    int id;
    int position;
    std::string name;
    std::string icon;
    std::string color;
};


#endif //TODOOS_PRIORITYENTITY_H
