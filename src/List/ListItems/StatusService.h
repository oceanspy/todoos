#ifndef TODOOS_STATUSSERVICE_H
#define TODOOS_STATUSSERVICE_H

#include <vector>
#include <string>
#include <stdexcept>
#include "../../Entities/StatusEntity.h"

class StatusService {
public:
    StatusService();
    enum Status {
        TO_DO = 0,
        STARTED = 1,
        REVIEWING = 2,
        PAUSED = 3,
        PENDING = 4,
        TENTATIVELY_CLOSED = 5,
        COMPLETED = 6,
        CANCELLED = 7
    };

    StatusEntity find(int statusId);
    StatusEntity getStatusFromName(const std::string& name);
    std::vector <std::string> getStatusesNames();
    std::string getCommandNamesAsString();
    bool isIdValid(int statusId);
    bool isNameValid(const std::string& statusName);
    int getIdFromName(const std::string& statusName);
    std::string getNameFromId(int statusId);
    bool isClosed(std::string statusName);
private:
    std::vector <StatusEntity> statuses;
    void make();
};


#endif //TODOOS_STATUSSERVICE_H
