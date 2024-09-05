#ifndef TODOOS_STATUSENTITY_H
#define TODOOS_STATUSENTITY_H

#include <string>
#include <vector>
#include "../Helpers/BashStyle.h"

class StatusEntity {
public:
    [[nodiscard]] const int * getId() const;
    [[nodiscard]] const int * getPosition() const;
    [[nodiscard]] const std::string * getName() const;
    [[nodiscard]] const std::string * getCommandName() const;
    [[nodiscard]] const std::string * getIcon() const;
    [[nodiscard]] const int getIconLength() const;
    [[nodiscard]] std::string getColor() const;
    [[nodiscard]] std::string getStyle() const;
    [[nodiscard]] const bool * isClosed() const;
    [[nodiscard]] const bool * isCancelled() const;
    [[nodiscard]] const bool * isPassive() const;
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
    int id = 0;
    int position = 0;
    std::string name;
    std::string command;
    std::string icon;
    int iconLength = 0;
    std::string color;
    std::string style;
    bool statusIsClosed = false;
    bool statusIsCancelled = false;
    bool statusIsPassive = false;
};


#endif //TODOOS_STATUSENTITY_H
