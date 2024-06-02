#include <algorithm>
#include "StatusService.h"

StatusService::StatusService()
{
    make();
}

void StatusService::make()
{
    // Note: Everything is string because we suppose that we are getting the data from storage
    // ID, position, name, icon, iconLength, color, style, statusIsClosed, statusIsCancelled, statusIsPassive
    statuses = {
            StatusEntity::setFromVector({std::to_string(TO_DO), "0", "to do", "to-do", "⏳", "2", "WHITE", "", "false", "false", "false"}),
            StatusEntity::setFromVector({std::to_string(STARTED), "1", "started", "started", "🏃", "1", "LIGHT_YELLOW", "ITALIC", "false", "false", "false"}),
            StatusEntity::setFromVector({std::to_string(REVIEWING), "2", "reviewing", "reviewing", "🔍", "1", "LIGHT_CYAN", "ITALIC", "false", "false", "true"}),
            StatusEntity::setFromVector({std::to_string(PAUSED), "3", "paused", "paused", "🚧", "1", "LIGHT_BLUE", "ITALIC", "false", "false", "true"}),
            StatusEntity::setFromVector({std::to_string(PENDING), "4", "pending", "pending", "🚧", "1", "MAGENTA", "ITALIC", "false", "false", "true"}),
//            StatusEntity::setFromVector({std::to_string(TENTATIVELY_CLOSED), "5", "tentat. closed", "tentatively-closed", "✅", "1", "MAGENTA", "ITALIC", "false", "false", "true"}),
            StatusEntity::setFromVector({std::to_string(COMPLETED), "6", "completed", "completed", "✅", "2", "GRAY", "", "true", "false", "true"}),
            StatusEntity::setFromVector({std::to_string(CANCELLED), "7", "cancelled", "cancelled", "🪧", "1", "GRAY", "", "true", "true", "true"})
    };
}

StatusEntity StatusService::find(int statusId)
{
    for (const StatusEntity& statusEntity : statuses)
    {
        if (statusEntity.getId() == statusId)
        {
            return statusEntity;
        }
    }

    throw std::invalid_argument("Status ID: " + std::to_string(statusId) + " is not valid.");

}

StatusEntity StatusService::getStatusFromName(const std::string &name)
{
    for (const StatusEntity& statusEntity : statuses)
    {
        if (statusEntity.getCommandName() == name)
        {
            return statusEntity;
        }
    }

    return StatusEntity::setFromVector({std::to_string(-1), "-1", "invalid", "invalid", "❓", "2", "WHITE", "", "false", "false", "false"});
}

std::vector <std::string> StatusService::getStatusesNames()
{
    std::vector <std::string> statusNames;
    statusNames.reserve(statuses.size());
    for (const StatusEntity& status : statuses)
    {
        statusNames.push_back(status.getName());
    }

    return statusNames;
}

std::string StatusService::getCommandNamesAsString()
{
    std::string statusesStr;
    for (const StatusEntity& status : statuses)
    {
        statusesStr += status.getCommandName() + " ";
    }

    statusesStr.pop_back(); // remove last space

    return statusesStr;
}

bool StatusService::isIdValid(int statusId)
{
    return std::ranges::any_of(statuses.begin(), statuses.end(), [statusId](const StatusEntity& statusEntity) {
        return statusEntity.getId() == statusId;
    });
}

bool StatusService::isNameValid(const std::string& statusName)
{
    return std::ranges::any_of(statuses.begin(), statuses.end(), [statusName](const StatusEntity& statusEntity) {
        return statusEntity.getCommandName() == statusName;
    });
}

int StatusService::getIdFromName(const std::string& statusName)
{
    for (const StatusEntity& statusEntity : statuses)
    {
        if (statusEntity.getCommandName() == statusName)
        {
            return statusEntity.getId();
        }
    }

    throw std::invalid_argument("Status: " + statusName + " is not valid.");
}

std::string StatusService::getNameFromId(int statusId)
{
    for (const StatusEntity& statusEntity : statuses)
    {
        if (statusEntity.getId() == statusId)
        {
            return statusEntity.getName();
        }
    }

    throw std::invalid_argument("Status ID: " + std::to_string(statusId) + " is not valid.");
}

bool StatusService::isClosed(std::string statusName)
{
    for (const StatusEntity& statusEntity : statuses)
    {
        if (statusEntity.getName() == statusName)
        {
            return statusEntity.isClosed();
        }
    }

    throw std::invalid_argument("Status: " + statusName + " is not valid.");
}
