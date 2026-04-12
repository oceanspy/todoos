#pragma once
#include <ctime>
#include <map>

struct ListCountSummary
{
    long total = 0;     // total active items
    long archived = 0;  // ALL items in archive list
    long delivered = 0; // COMPLETED in active + COMPLETED in archive
    long cancelled = 0; // CANCELLED in active + CANCELLED in archive
    long deleted = 0;   // ALL items in delete list
    time_t lastUpdate = 0;

    void addStatus(int statusId) { statusCounts[statusId]++; }
    void addPriority(int priorityId) { priorityCounts[priorityId]++; }
    void lastUpdatedAt(time_t timestamp) { lastUpdate = timestamp; }

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

    time_t getLastUpdate() const { return lastUpdate; }

  private:
    std::map<int, long> statusCounts;
    std::map<int, long> priorityCounts;
};
