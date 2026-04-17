#include "../../FileDataStorageRepositories/ListRepository.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"
#include "../../List/ListService.h"
#include "../../Serializers/ConfSerializer.h"
#include "../../Serializers/JsonSerializer.h"
#include "../Mock/MockAppInitialization.h"
#include "../Mock/MockAppInstallation.h"
#include <catch2/catch_test_macros.hpp>

// Seeded items in the default list (from MockAppInstallation):
// "aaaa" — HIGH priority, TO_DO status
// "bbbb" — MEDIUM priority, STARTED status

TEST_CASE("ListItemService::getCountSummary", "[ListItemService]")
{
    IOService ioService("cli");
    ConfSerializer confService = ConfSerializer(ioService);
    JsonSerializer jsonService = JsonSerializer(ioService);
    std::unique_ptr<DataSerializerInterface> fileDataStorageServicePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> fileDataConfigStorageServicePtr =
        std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_getCountSummaryTest");
    MockAppInstallation installation(ioService, jsonService, confService, init);

    installation.wipe();
    installation.make();

    EventBus bus = EventBus();
    Command command = Command("", {}, {}, "");
    ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
    ConfigService configService(ioService, init, configRepository, cacheRepository, command);

    PriorityService priorityService = PriorityService();
    StatusService statusService = StatusService();
    ListItemRepository listItemRepository(
        configService, fileDataStorageServicePtr.get(), priorityService, statusService);
    ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
    ListRepository listRepository(configService, fileDataStorageServicePtr.get());
    ListService listService(ioService, configService, listRepository, bus);

    ListName listName = listService.createUsedListName();
    ListName listNameArchive = ListName::createVariant(listName, "archive");
    ListName listNameDelete = ListName::createVariant(listName, "delete");

    SECTION("single list — baseline active items only")
    {
        ListCountSummary summary = listItemService.getCountSummary({ listName });

        REQUIRE(summary.total == 2);
        REQUIRE(summary.archived == 0);
        REQUIRE(summary.delivered == 0);
        REQUIRE(summary.cancelled == 0);
        REQUIRE(summary.deleted == 0);

        REQUIRE(summary.getStatus(StatusService::QUEUED) == 1);
        REQUIRE(summary.getStatus(StatusService::STARTED) == 1);
        REQUIRE(summary.getStatus(StatusService::COMPLETED) == 0);
        REQUIRE(summary.getStatus(StatusService::CANCELLED) == 0);

        REQUIRE(summary.getPriority(PriorityService::HIGH) == 1);
        REQUIRE(summary.getPriority(PriorityService::MEDIUM) == 1);
        REQUIRE(summary.getPriority(PriorityService::CRITICAL) == 0);
        REQUIRE(summary.getPriority(PriorityService::LOW) == 0);
    }

    SECTION("archived items are counted in archived, not in total or statusCounts")
    {
        listItemService.archive("aaaa", listName);

        ListCountSummary summary = listItemService.getCountSummary({ listName });

        REQUIRE(summary.total == 1);
        REQUIRE(summary.archived == 1);
        REQUIRE(summary.delivered == 0);
        REQUIRE(summary.cancelled == 0);
        REQUIRE(summary.deleted == 0);

        REQUIRE(summary.getStatus(StatusService::QUEUED) == 0);
        REQUIRE(summary.getStatus(StatusService::STARTED) == 1);
        REQUIRE(summary.getPriority(PriorityService::HIGH) == 0);
        REQUIRE(summary.getPriority(PriorityService::MEDIUM) == 1);
    }

    SECTION("COMPLETED active items contribute to delivered and statusCounts")
    {
        const int completedStatus = StatusService::COMPLETED;
        listItemService.editStatus("aaaa", listName, &completedStatus);

        ListCountSummary summary = listItemService.getCountSummary({ listName });

        REQUIRE(summary.total == 2);
        REQUIRE(summary.delivered == 1);
        REQUIRE(summary.cancelled == 0);
        REQUIRE(summary.getStatus(StatusService::COMPLETED) == 1);
        REQUIRE(summary.getStatus(StatusService::QUEUED) == 0);
    }

    SECTION("COMPLETED archived items contribute to delivered but not to statusCounts")
    {
        listItemService.archive("aaaa", listName);

        const int completedStatus = StatusService::COMPLETED;
        listItemService.editStatus("aaaa", listNameArchive, &completedStatus);

        ListCountSummary summary = listItemService.getCountSummary({ listName });

        REQUIRE(summary.archived == 1);
        REQUIRE(summary.delivered == 1);
        REQUIRE(summary.getStatus(StatusService::COMPLETED) == 0); // not in active statusCounts
    }

    SECTION("CANCELLED active items contribute to cancelled and statusCounts")
    {
        const int cancelledStatus = StatusService::CANCELLED;
        listItemService.editStatus("bbbb", listName, &cancelledStatus);

        ListCountSummary summary = listItemService.getCountSummary({ listName });

        REQUIRE(summary.total == 2);
        REQUIRE(summary.cancelled == 1);
        REQUIRE(summary.delivered == 0);
        REQUIRE(summary.getStatus(StatusService::CANCELLED) == 1);
        REQUIRE(summary.getStatus(StatusService::STARTED) == 0);
    }

    SECTION("CANCELLED archived items contribute to cancelled but not to statusCounts")
    {
        listItemService.archive("aaaa", listName);

        const int cancelledStatus = StatusService::CANCELLED;
        listItemService.editStatus("aaaa", listNameArchive, &cancelledStatus);

        ListCountSummary summary = listItemService.getCountSummary({ listName });

        REQUIRE(summary.archived == 1);
        REQUIRE(summary.cancelled == 1);
        REQUIRE(summary.getStatus(StatusService::CANCELLED) == 0); // not in active statusCounts
    }

    SECTION("soft-deleted items are counted in deleted, not in total")
    {
        listItemService.softDelete("aaaa", listName);

        ListCountSummary summary = listItemService.getCountSummary({ listName });

        REQUIRE(summary.total == 1);
        REQUIRE(summary.deleted == 1);
        REQUIRE(summary.archived == 0);
        REQUIRE(summary.getStatus(StatusService::QUEUED) == 0);
        REQUIRE(summary.getStatus(StatusService::STARTED) == 1);
    }

    SECTION("multiple lists aggregate all counts correctly")
    {
        installation.createListFile("secondList");
        listService.add("secondList", "default", "default");
        ListName secondListName = listService.createListName("secondList");

        const std::string lowPriority = "low";
        const std::string criticalPriority = "critical";
        const std::string todoStatus = "queued";
        listItemService.add(secondListName, "extra item 1", &lowPriority, &todoStatus);
        listItemService.add(secondListName, "extra item 2", &criticalPriority, &todoStatus);

        ListCountSummary summary = listItemService.getCountSummary({ listName, secondListName });

        REQUIRE(summary.total == 4);
        REQUIRE(summary.getStatus(StatusService::QUEUED) == 3);   // 1 from list + 2 from secondList
        REQUIRE(summary.getStatus(StatusService::STARTED) == 1); // 1 from list
        REQUIRE(summary.getPriority(PriorityService::LOW) == 1);
        REQUIRE(summary.getPriority(PriorityService::CRITICAL) == 1);
        REQUIRE(summary.getPriority(PriorityService::HIGH) == 1);
        REQUIRE(summary.getPriority(PriorityService::MEDIUM) == 1);
    }

    SECTION("empty list returns all zeros")
    {
        installation.createListFile("emptyList");
        listService.add("emptyList", "default", "default");
        ListName emptyListName = listService.createListName("emptyList");

        ListCountSummary summary = listItemService.getCountSummary({ emptyListName });

        REQUIRE(summary.total == 0);
        REQUIRE(summary.archived == 0);
        REQUIRE(summary.delivered == 0);
        REQUIRE(summary.cancelled == 0);
        REQUIRE(summary.deleted == 0);
        REQUIRE(summary.getStatus(StatusService::QUEUED) == 0);
        REQUIRE(summary.getPriority(PriorityService::HIGH) == 0);
    }
}
