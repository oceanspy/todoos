#ifndef TODOOS_STATUSSERVICE_H
#define TODOOS_STATUSSERVICE_H

#include "StatusEntity.h"
#include <algorithm>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

class StatusService
{
  public:
    StatusService();
    enum Status
    {
        QUEUED = 0,
        STARTED = 1,
        TRIAGED = 2,
        PAUSED = 3,
        BLOCKED = 4,
        COMPLETED = 6,
        CANCELLED = 7
    };

    StatusEntity find(int statusId);
    StatusEntity getStatusFromName(const std::string& name);
    std::vector<std::string> getStatusesNames();
    std::string getCommandNamesAsString();
    bool isIdValid(int statusId);
    bool isNameValid(const std::string& statusName);
    int getIdFromName(const std::string& statusName);
    std::string getNameFromId(int statusId);
    bool isClosed(std::string statusName);

  private:
    std::vector<StatusEntity> statuses;
    void make();
};

#endif // TODOOS_STATUSSERVICE_H
