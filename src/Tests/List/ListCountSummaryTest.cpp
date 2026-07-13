#include "../../List/ListCountSummary.h"
#include "../../List/ListItems/PriorityService.h"
#include "../../List/ListItems/StatusService.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ListCountSummary", "[ListCountSummary]")
{
    SECTION("direct fields default to 0")
    {
        ListCountSummary summary;
        REQUIRE(summary.total == 0);
        REQUIRE(summary.archived == 0);
        REQUIRE(summary.delivered == 0);
        REQUIRE(summary.cancelled == 0);
        REQUIRE(summary.deleted == 0);
    }

    SECTION("getStatus returns 0 for unknown key")
    {
        ListCountSummary summary;
        REQUIRE(summary.getStatus(StatusService::QUEUED) == 0);
        REQUIRE(summary.getStatus(StatusService::COMPLETED) == 0);
        REQUIRE(summary.getStatus(999) == 0);
    }

    SECTION("addStatus increments the right key")
    {
        ListCountSummary summary;
        summary.addStatus(StatusService::QUEUED);
        summary.addStatus(StatusService::QUEUED);
        summary.addStatus(StatusService::STARTED);

        REQUIRE(summary.getStatus(StatusService::QUEUED) == 2);
        REQUIRE(summary.getStatus(StatusService::STARTED) == 1);
        REQUIRE(summary.getStatus(StatusService::PAUSED) == 0);
    }

    SECTION("getStatus returns correct count after multiple adds")
    {
        ListCountSummary summary;
        for (int i = 0; i < 5; i++) {
            summary.addStatus(StatusService::TRIAGED);
        }
        REQUIRE(summary.getStatus(StatusService::TRIAGED) == 5);
    }

    SECTION("getPriority returns 0 for unknown key")
    {
        ListCountSummary summary;
        REQUIRE(summary.getPriority(PriorityService::LOW) == 0);
        REQUIRE(summary.getPriority(PriorityService::CRITICAL) == 0);
        REQUIRE(summary.getPriority(999) == 0);
    }

    SECTION("addPriority increments the right key")
    {
        ListCountSummary summary;
        summary.addPriority(PriorityService::HIGH);
        summary.addPriority(PriorityService::HIGH);
        summary.addPriority(PriorityService::CRITICAL);

        REQUIRE(summary.getPriority(PriorityService::HIGH) == 2);
        REQUIRE(summary.getPriority(PriorityService::CRITICAL) == 1);
        REQUIRE(summary.getPriority(PriorityService::LOW) == 0);
    }

    SECTION("getPriority returns correct count after multiple adds")
    {
        ListCountSummary summary;
        for (int i = 0; i < 3; i++) {
            summary.addPriority(PriorityService::URGENT);
        }
        REQUIRE(summary.getPriority(PriorityService::URGENT) == 3);
    }

    SECTION("status and priority maps do not interfere with each other")
    {
        // COMPLETED=6 has no matching priority value; LOW=0 has no matching status value
        // This ensures cross-map lookups always find 0
        ListCountSummary summary;
        summary.addStatus(StatusService::COMPLETED);
        summary.addPriority(PriorityService::LOW);

        REQUIRE(summary.getStatus(StatusService::COMPLETED) == 1);
        REQUIRE(summary.getPriority(PriorityService::LOW) == 1);
        REQUIRE(summary.getStatus(PriorityService::LOW) == 0);
        REQUIRE(summary.getPriority(StatusService::COMPLETED) == 0);
    }

    SECTION("all status keys are independent")
    {
        ListCountSummary summary;
        summary.addStatus(StatusService::QUEUED);
        summary.addStatus(StatusService::STARTED);
        summary.addStatus(StatusService::TRIAGED);
        summary.addStatus(StatusService::PAUSED);
        summary.addStatus(StatusService::BLOCKED);
        summary.addStatus(StatusService::COMPLETED);
        summary.addStatus(StatusService::CANCELLED);

        REQUIRE(summary.getStatus(StatusService::QUEUED) == 1);
        REQUIRE(summary.getStatus(StatusService::STARTED) == 1);
        REQUIRE(summary.getStatus(StatusService::TRIAGED) == 1);
        REQUIRE(summary.getStatus(StatusService::PAUSED) == 1);
        REQUIRE(summary.getStatus(StatusService::BLOCKED) == 1);
        REQUIRE(summary.getStatus(StatusService::COMPLETED) == 1);
        REQUIRE(summary.getStatus(StatusService::CANCELLED) == 1);
    }

    SECTION("all priority keys are independent")
    {
        ListCountSummary summary;
        summary.addPriority(PriorityService::LOW);
        summary.addPriority(PriorityService::MEDIUM);
        summary.addPriority(PriorityService::HIGH);
        summary.addPriority(PriorityService::URGENT);
        summary.addPriority(PriorityService::CRITICAL);

        REQUIRE(summary.getPriority(PriorityService::LOW) == 1);
        REQUIRE(summary.getPriority(PriorityService::MEDIUM) == 1);
        REQUIRE(summary.getPriority(PriorityService::HIGH) == 1);
        REQUIRE(summary.getPriority(PriorityService::URGENT) == 1);
        REQUIRE(summary.getPriority(PriorityService::CRITICAL) == 1);
    }
}
