#ifndef TODOOS_STATUSENTITY_H
#define TODOOS_STATUSENTITY_H

#include <string>
#include <vector>
#include "../Helpers/BashStyle.h"

class StatusEntity {
public:
    [[nodiscard]] int getId() const;
    [[nodiscard]] int getPosition() const;
    [[nodiscard]] std::string getName() const;
    [[nodiscard]] std::string getCommandName() const;
    [[nodiscard]] std::string getIcon() const;
    [[nodiscard]] int getIconLength() const;
    [[nodiscard]] std::string getColor() const;
    [[nodiscard]] std::string getStyle() const;
    [[nodiscard]] bool isClosed() const;
    [[nodiscard]] bool isCancelled() const;
    [[nodiscard]] bool isPassive() const;
    void setId(int id);
    void setPosition(int position);
    void setName(std::string name);
    void setCommandName(std::string command);
    void setIcon(std::string icon);
    void setIconLength(int iconLength);
    void setColor(std::string color);
    void setStyle(std::string style);
    void setClosed(bool closed);
    void setCancelled(bool cancelled);
    void setPassive(bool passive);
    static StatusEntity setFromVector(std::vector <std::string> item);
    static std::vector <std::string> makeVector(const StatusEntity& priorityEntity);
private:
    int id;
    int position;
    std::string name;
    std::string command;
    std::string icon;
    int iconLength;
    std::string color;
    std::string style;
    bool statusIsClosed;
    bool statusIsCancelled;
    bool statusIsPassive;
};


#endif //TODOOS_STATUSENTITY_H
