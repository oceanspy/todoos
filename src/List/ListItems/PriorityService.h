#ifndef TODOOS_PRIORITYSERVICE_H
#define TODOOS_PRIORITYSERVICE_H


#include <vector>
#include <string>
#include <stdexcept>
#include "../../Entities/PriorityEntity.h"

class PriorityService {
public:
    PriorityService();
    enum Priority {
        LOW = 0,
        MEDIUM = 1,
        HIGH = 2,
        URGENT = 3,
        CRITICAL = 4
    };

    PriorityEntity find(int priorityId);
    PriorityEntity getPriorityFromName(const std::string& name);
    std::vector <std::string> getPrioritiesNames();
    std::string getNamesAsString();
    bool isIdValid(int priorityId);
    bool isNameValid(const std::string& priorityName);
    int getIdFromName(const std::string& priority);
    std::string getNameFromId(int priorityId);
    bool isMax(const std::string& priorityName);
    bool isMin(const std::string& priorityName);
private:
    std::vector <PriorityEntity> priorities;
    void make();
};


#endif //TODOOS_PRIORITYSERVICE_H
