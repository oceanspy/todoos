#include <algorithm>
#include "PriorityService.h"

PriorityService::PriorityService()
{
    make();
}

void PriorityService::make()
{
    // Note: Everything is string because we suppose that we are getting the data from storage
    // ID, position, name, icon, color
    priorities = {
        PriorityEntity::setFromVector({ std::to_string(LOW), "0", "low", "▽", "GREEN"}),
        PriorityEntity::setFromVector({ std::to_string(MEDIUM), "1", "medium", "▼", "LIGHT_GREEN"}),
        PriorityEntity::setFromVector({ std::to_string(HIGH), "2", "high", "▶", "ORANGE"}),
        PriorityEntity::setFromVector({ std::to_string(URGENT), "3", "urgent", "▲", "RED"}),
        PriorityEntity::setFromVector({ std::to_string(CRITICAL), "4", "critical", "■", "WHITE"})
    };
}

PriorityEntity PriorityService::find (int priorityId)
{
    for (const PriorityEntity& priorityEntity : priorities)
    {
        if (*priorityEntity.getId() == priorityId)
        {
            return priorityEntity;
        }
    }

    throw std::invalid_argument("Priority ID: " + std::to_string(priorityId) + " is not valid.");
}

PriorityEntity PriorityService::getPriorityFromName(const std::string& name)
{
    for (const PriorityEntity& priority : priorities)
    {
        if (*priority.getName() == name)
        {
            return priority;
        }
    }

    return PriorityEntity::setFromVector({std::to_string(-1), "-1", "invalid", "▽", "WHITE"});
}

std::vector <std::string> PriorityService::getPrioritiesNames()
{
    std::vector <std::string> priorityNames;
    priorityNames.reserve(priorities.size());
    for (const PriorityEntity& priority : priorities)
    {
        priorityNames.push_back(*priority.getName());
    }

    return priorityNames;
}

std::string PriorityService::getNamesAsString()
{
    std::string prioritiesStr;
    for (const PriorityEntity& priority : priorities)
    {
        prioritiesStr += *priority.getName() + " ";
    }

    prioritiesStr.pop_back(); // remove last space

    return prioritiesStr;
}

bool PriorityService::isIdValid(int priorityId)
{
    return std::any_of(priorities.begin(), priorities.end(), [priorityId](const PriorityEntity& priorityEntity) {
        return *priorityEntity.getId() == priorityId;
    });
}

bool PriorityService::isNameValid(const std::string& priorityName)
{
    return std::any_of(priorities.begin(), priorities.end(), [priorityName](const PriorityEntity& priorityEntity) {
        return *priorityEntity.getName() == priorityName;
    });
}

int PriorityService::getIdFromName(const std::string& priority)
{
    for (const PriorityEntity& priorityEntity : priorities)
    {
        if (*priorityEntity.getName() == priority)
        {
            return *priorityEntity.getId();
        }
    }

    throw std::invalid_argument("Priority: " + priority + " is not valid.");
}

std::string PriorityService::getNameFromId(int priorityId)
{
    for (const PriorityEntity& priorityEntity : priorities)
    {
        if (*priorityEntity.getId() == priorityId)
        {
            return *priorityEntity.getName();
        }
    }

    throw std::invalid_argument("Priority ID: " + std::to_string(priorityId) + " is not valid.");
}

bool PriorityService::isMax(const std::string& priorityName)
{
    int priorityInt = getIdFromName(priorityName);
    priorityInt++;
    if (isIdValid(priorityInt))
    {
        return false;
    }
    return true;
}

bool PriorityService::isMin(const std::string& priorityName)
{
    int priorityInt = getIdFromName(priorityName);
    priorityInt--;
    if (isIdValid(priorityInt))
    {
        return false;
    }
    return true;
}
