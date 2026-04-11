#pragma once
#include <map>

struct ListCountSummary
{
    long total     = 0;  // total active items
    long archived  = 0;  // ALL items in archive list
    long delivered = 0;  // COMPLETED in active + COMPLETED in archive
    long cancelled = 0;  // CANCELLED in active + CANCELLED in archive
    long deleted   = 0;  // ALL items in delete list

    void addStatus(int statusId) { statusCounts[statusId]++; }
    void addPriority(int priorityId) { priorityCounts[priorityId]++; }

    long getStatus(int statusId) const
    {
        auto it = statusCounts.find(statusId);
        return it != statusCounts.end() ? it->second : 0L;
    }

    long getPriority(int priorityId) const
    {
        auto it = priorityCounts.find(priorityId);
        return it != priorityCounts.end() ? it->second : 0L;
    }

  private:
    std::map<int, long> statusCounts;
    std::map<int, long> priorityCounts;
};
