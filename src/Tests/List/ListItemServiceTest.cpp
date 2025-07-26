#include "../../List/ListItemService.h"
#include "../../FileDataStorage/ConfService.h"
#include "../../FileDataStorage/JSONService.h"
#include "../../FileDataStorageRepositories/ListRepository.h"
#include "../../IOService/IOService.h"
#include "../../List/ListService.h"
#include "../Mock/MockInit.h"
#include "../Mock/MockInstallation.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>

// {"aaaa", "test 1", "3", "0", "1712487259", "1712487259", "0", "0"}
// {"bbbb", "test 2", "1", "1", "1712487272", "1712487272", "1712487259", "0"}

TEST_CASE("ListItemServiceTest", "[ListItemService]")
{
    // Create mock objects
    IOService ioService("cli");
    ConfService confService = ConfService(ioService);
    JSONService jsonService = JSONService(ioService);
    std::unique_ptr<FileDataServiceInterface> fileDataStorageServicePtr = std::make_unique<JSONService>(ioService);
    std::unique_ptr<FileDataServiceInterface> fileDataConfigStorageServicePtr =
        std::make_unique<ConfService>(ioService);
    MockInit init(ioService, "_todoos_ListItemServiceTest");
    MockInstallation installation(ioService, jsonService, confService, init);
    std::string tempListNameStr = "tempListName";
    std::string tempList2NameStr = "tempList2Name";

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
    ListName tempListName = listService.createListName(tempListNameStr);
    ListName tempListNameArchive = ListName::createVariant(tempListName, "archive");
    ListName tempListNameDelete = ListName::createVariant(tempListName, "delete");
    ListName tempListName2 = listService.createListName(tempList2NameStr);
    ListName tempListName2Archive = ListName::createVariant(tempListName2, "archive");
    ListName tempListName2Delete = ListName::createVariant(tempListName2, "delete");

    SECTION("get")
    {
        std::vector<ListItemEntity> listItems = listItemService.get(listName);
        REQUIRE(listItems.size() == 2);
        REQUIRE(*listItems[0].getId() == "aaaa");
        REQUIRE(*listItems[1].getId() == "bbbb");
    }

    SECTION("get + filters")
    {
        std::vector<ListItemEntity> listItems = listItemService.get(listName);
        listItemService.filterPriorityAbove(listItems, 0);
        REQUIRE(listItems.size() == 2);
        REQUIRE(*listItems[0].getId() == "aaaa");
        REQUIRE(*listItems[1].getId() == "bbbb");
        listItemService.filterPriorityAbove(listItems, 2);
        REQUIRE(listItems.size() == 1);
        REQUIRE(*listItems[0].getId() == "aaaa");

        std::vector<ListItemEntity> listItems2 = listItemService.get(listName);
        listItemService.filterDeadlineBefore(listItems2, 1712487260);
        REQUIRE(listItems2.size() == 1);
        REQUIRE(*listItems2[0].getId() == "bbbb");

        std::vector<ListItemEntity> listItems3 = listItemService.get(listName);
        listItemService.filterStatus(listItems3, { 0, 1 });
        REQUIRE(listItems3.size() == 2);
        REQUIRE(*listItems3[0].getId() == "aaaa");
        REQUIRE(*listItems3[1].getId() == "bbbb");

        std::vector<ListItemEntity> listItems4 = listItemService.get(listName);
        listItemService.filterStatus(listItems4, { 0 });
        REQUIRE(listItems4.size() == 1);
        REQUIRE(*listItems4[0].getId() == "aaaa");
    }

    SECTION("find")
    {
        ListItemEntity listItemEntity = listItemService.find("aaaa", listName);
        REQUIRE(*listItemEntity.getId() == "aaaa");
        REQUIRE(*listItemEntity.getValue() == "test 1");
        REQUIRE(*(*listItemEntity.priority()).getName() == "high");
        REQUIRE(*(*listItemEntity.status()).getCommandName() == "to-do");
        REQUIRE(*(*listItemEntity.priority()).getName() == "high");
        REQUIRE(*listItemEntity.getCreatedAt() == 1712487259);
        REQUIRE(*listItemEntity.getUpdatedAt() == 1712487259);
    }

    SECTION("add")
    {
        std::string priorityValue = "high";
        const std::string* priority = &priorityValue;
        std::string statusValue = "to-do";
        const std::string* status = &statusValue;
        std::string value = "test 1";

        std::string id = listItemService.add(listName, value, priority, status);
        ListItemEntity listItemEntity = listItemService.find(id, listName);
        REQUIRE(*listItemEntity.getId() == id);

        std::string id2 = listItemService.add(listName, value);
        ListItemEntity listItemEntity2 = listItemService.find(id2, listName);
        REQUIRE(*listItemEntity2.getId() == id2);
        REQUIRE(*(*listItemEntity2.priority()).getName() == "medium");
        REQUIRE(*(*listItemEntity2.status()).getCommandName() == "to-do");
    }

    SECTION("add too long")
    {
        std::string priorityValue = "high";
        const std::string* priority = &priorityValue;
        std::string statusValue = "to-do";
        const std::string* status = &statusValue;
        std::string value =
            "long string of more than 255 characters xxxxxxxxxxxxxxxxxxxxxxx xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx "
            "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx "
            "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
        REQUIRE_THROWS(listItemService.add(listName, value, priority, status));
    }

    SECTION("add with deadline")
    {
        std::string priorityValue = "high";
        const std::string* priority = &priorityValue;
        std::string statusValue = "to-do";
        const std::string* status = &statusValue;
        std::string value = "test 1";
        std::string deadline = "2024-12-31";
        time_t dueAt = DateHelpers::relativeDateToTimestamp(deadline);

        std::string id = listItemService.add(listName, value, priority, status, dueAt);
        ListItemEntity listItemEntity = listItemService.find(id, listName);
        REQUIRE(*listItemEntity.getId() == id);
    }

    SECTION("edit")
    {
        std::string id = "aaaa";
        std::string priorityValue = "high";
        const std::string* priority = &priorityValue;
        std::string statusValue = "to-do";
        const std::string* status = &statusValue;
        std::string value = "test 1";

        std::string priorityNewValue = "low";
        const std::string* newPriority = &priorityNewValue;
        std::string statusNewValue = "started";
        const std::string* newStatus = &statusNewValue;
        std::string newValue = "test 1 update";
        listItemService.edit(id, listName, newValue, newPriority, newStatus);

        ListItemEntity listItemEntity = listItemService.find(id, listName);
        REQUIRE(*listItemEntity.getId() == "aaaa");
        REQUIRE(*listItemEntity.getValue() == "test 1 update");
        REQUIRE(*(*listItemEntity.priority()).getName() == "low");
        REQUIRE(*(*listItemEntity.status()).getCommandName() == "started");
        REQUIRE(*listItemEntity.getCreatedAt() == 1712487259);

        listItemService.edit(id, listName, "update only value");
        listItemEntity = listItemService.find(id, listName);
        REQUIRE(*listItemEntity.getId() == "aaaa");
        REQUIRE(*listItemEntity.getValue() == "update only value");
        REQUIRE(*(*listItemEntity.priority()).getName() == "low");
        REQUIRE(*(*listItemEntity.status()).getCommandName() == "started");
        REQUIRE(*listItemEntity.getCreatedAt() == 1712487259);

        listItemService.edit(id, listName, value, priority, status);

        listItemEntity = listItemService.find("aaaa", listName);
        REQUIRE(*listItemEntity.getValue() == "test 1");
        REQUIRE(*(*listItemEntity.priority()).getName() == "high");
        REQUIRE(*(*listItemEntity.status()).getCommandName() == "to-do");
        REQUIRE(*listItemEntity.getCreatedAt() == 1712487259);
    }

    SECTION("close item (complete/cancel)")
    {
        std::string id = "aaaa";

        const int statusValue = StatusService::COMPLETED;
        const int* status = &statusValue;
        listItemService.editStatus(id, listName, status);

        ListItemEntity listItemEntity = listItemService.find(id, listName);
        REQUIRE(*listItemEntity.getClosedAt() > 0);

        std::string id2 = "bbbb";

        const int statusValue2 = StatusService::CANCELLED;
        const int* status2 = &statusValue2;
        listItemService.editStatus(id, listName, status2);

        ListItemEntity listItemEntity2 = listItemService.find(id2, listName);
        REQUIRE(*listItemEntity.getClosedAt() > 0);
    }

    SECTION("editStatus")
    {
        std::string id = "aaaa";
        const int statusValue = StatusService::TO_DO;
        const int* status = &statusValue;
        std::string value = "test 1";

        const int newStatusValue = StatusService::STARTED;
        const int* newStatus = &newStatusValue;
        std::string newValue = "test 1 update";

        listItemService.editStatus(id, listName, newStatus);

        ListItemEntity listItemEntity = listItemService.find("aaaa", listName);
        REQUIRE(*listItemEntity.getId() == "aaaa");
        REQUIRE(*listItemEntity.getValue() == "test 1");
        REQUIRE(*(*listItemEntity.priority()).getName() == "high");
        REQUIRE(*(*listItemEntity.status()).getCommandName() == "started");
        REQUIRE(*listItemEntity.getCreatedAt() == 1712487259);

        listItemService.editStatus(id, listName, status);

        listItemEntity = listItemService.find("aaaa", listName);
        REQUIRE(*listItemEntity.getValue() == "test 1");
        REQUIRE(*(*listItemEntity.priority()).getName() == "high");
        REQUIRE(*(*listItemEntity.status()).getCommandName() == "to-do");
        REQUIRE(*listItemEntity.getCreatedAt() == 1712487259);
    }

    SECTION("editStatus to closed with auto archive")
    {
        configService.edit("archiveWhenCompleted", "true");
        std::vector<ConfigEntity> configs = configService.get();

        std::string id = "aaaa";

        const int newStatusValue = StatusService::COMPLETED;
        const int* newStatus = &newStatusValue;
        std::string newValue = "test 1 update";

        listItemService.editStatus(id, listName, newStatus);

        REQUIRE_THROWS(listItemService.find("aaaa", listName));
        REQUIRE_NOTHROW(listItemService.find("aaaa", listNameArchive));
    }

    SECTION("add & remove")
    {
        std::string priorityValue = "low";
        const std::string* priority = &priorityValue;
        std::string statusValue = "to-do";
        const std::string* status = &statusValue;
        std::string value = "test 3";
        std::string id = listItemService.add(tempListName, value, priority, status);

        ListItemEntity listItemEntity = listItemService.find(id, tempListName);
        REQUIRE(*listItemEntity.getId() == id);
        REQUIRE(*listItemEntity.getValue() == "test 3");
        REQUIRE(*(*listItemEntity.priority()).getName() == "low");
        REQUIRE(*(*listItemEntity.status()).getCommandName() == "to-do");

        std::vector<ListItemEntity> listItems = listItemService.get(tempListName);
        REQUIRE(listItems.size() == 3);
        REQUIRE(*listItems[0].getId() == "aaaa");
        REQUIRE(*listItems[1].getId() == "bbbb");
        REQUIRE(*listItems[2].getId() == id);

        listItemService.softDelete(id, tempListName);

        std::vector<ListItemEntity> listDelItems = listItemService.get(tempListNameDelete);
        REQUIRE(listDelItems.size() == 1);
        REQUIRE(*listDelItems[0].getId() == id);

        std::vector<ListItemEntity> listItems2 = listItemService.get(tempListName);
        REQUIRE(listItems2.size() == 2);
        REQUIRE(*listItems2[0].getId() == "aaaa");
        REQUIRE(*listItems2[1].getId() == "bbbb");

        listItemService.restore(id, tempListName);
        std::vector<ListItemEntity> listRestoreItems = listItemService.get(tempListName);
        REQUIRE(listRestoreItems.size() == 3);
        REQUIRE(*listRestoreItems[0].getId() == "aaaa");
        REQUIRE(*listRestoreItems[1].getId() == "bbbb");
        REQUIRE(*listRestoreItems[2].getId() == id);
    }

    SECTION("Search")
    {
        std::vector<std::string> searchValues = { "test" };
        std::vector<ListItemEntity> listItems = listItemService.search(listName, searchValues);
        REQUIRE(listItems.size() == 2);
        REQUIRE(*listItems[0].getId() == "aaaa");
        REQUIRE(*listItems[1].getId() == "bbbb");

        std::string priorityValue = "high";
        const std::string* priority = &priorityValue;
        std::string statusValue = "to-do";
        const std::string* status = &statusValue;
        std::string value = "bonjour tout le monde, ça marche?";
        std::string id = listItemService.add(listName, value, priority, status);

        searchValues = { "bonjour" };
        std::vector<ListItemEntity> listItems2 = listItemService.search(listName, searchValues);
        REQUIRE(listItems2.size() == 1);
        REQUIRE(*listItems2[0].getId() == id);

        searchValues = { "monde" };
        std::vector<ListItemEntity> listItems3 = listItemService.search(listName, searchValues);
        REQUIRE(listItems3.size() == 1);
        REQUIRE(*listItems3[0].getId() == id);

        searchValues = { "truc" };
        REQUIRE_THROWS(listItemService.search(listName, searchValues));

        listItemService.remove(id, listName);
    }

    SECTION("append")
    {
        std::string id = "aaaa";
        std::string value = "append test";
        listItemService.append(id, listName, value);

        ListItemEntity listItemEntity = listItemService.find(id, listName);
        REQUIRE(*listItemEntity.getId() == id);
        REQUIRE(*listItemEntity.getValue() == "test 1 append test");
        REQUIRE(*(*listItemEntity.priority()).getName() == "high");
        REQUIRE(*(*listItemEntity.status()).getCommandName() == "to-do");
        REQUIRE(*listItemEntity.getCreatedAt() == 1712487259);

        std::string priorityValue = "high";
        const std::string* priority = &priorityValue;
        std::string statusValue = "to-do";
        const std::string* status = &statusValue;
        listItemService.edit(id, listName, "test 1", priority, status);
    }

    SECTION("prepend")
    {
        std::string id = "aaaa";
        std::string value = "prepend";
        listItemService.prepend(id, listName, value);

        ListItemEntity listItemEntity = listItemService.find(id, listName);
        REQUIRE(*listItemEntity.getId() == id);
        REQUIRE(*listItemEntity.getValue() == "prepend test 1");
        REQUIRE(*(*listItemEntity.priority()).getName() == "high");
        REQUIRE(*(*listItemEntity.status()).getCommandName() == "to-do");
        REQUIRE(*listItemEntity.getCreatedAt() == 1712487259);

        std::string priorityValue = "high";
        const std::string* priority = &priorityValue;
        std::string statusValue = "to-do";
        const std::string* status = &statusValue;
        listItemService.edit(id, listName, "test 1", priority, status);
    }

    SECTION("increase/decrease priority")
    {
        std::string id = "aaaa";

        listItemService.increasePriority(id, listName);
        ListItemEntity listItemEntity = listItemService.find(id, listName);
        REQUIRE(*(*listItemEntity.priority()).getName() == "urgent");

        REQUIRE_NOTHROW(listItemService.increasePriority(id, listName));
        REQUIRE_THROWS(listItemService.increasePriority(id, listName));

        listItemService.decreasePriority(id, listName);
        listItemService.decreasePriority(id, listName);
        listItemService.decreasePriority(id, listName);
        listItemService.decreasePriority(id, listName);
        REQUIRE_THROWS(listItemService.decreasePriority(id, listName));
        listItemService.increasePriority(id, listName);
        listItemService.increasePriority(id, listName);
        listItemService.increasePriority(id, listName);
        ListItemEntity listItemEntity3 = listItemService.find(id, listName);
        REQUIRE(*(*listItemEntity.priority()).getName() == "urgent");
    }

    SECTION("set priority")
    {
        std::string id = "aaaa";

        std::string priorityValue = "critical";
        const std::string* priority = &priorityValue;
        listItemService.setPriority(id, listName, priority);
        ListItemEntity listItemEntity = listItemService.find(id, listName);
        REQUIRE(*(*listItemEntity.priority()).getName() == "critical");

        REQUIRE_THROWS(listItemService.setPriority(id, listName, priority));

        std::string priorityValue2 = "low";
        const std::string* priority2 = &priorityValue2;
        listItemService.setPriority(id, listName, priority2);
        ListItemEntity listItemEntity2 = listItemService.find(id, listName);
        REQUIRE(*(*listItemEntity2.priority()).getName() == "low");
    }

    SECTION("set status")
    {
        std::string id = "aaaa";

        std::string statusValue = "paused";
        const std::string* status = &statusValue;
        listItemService.setStatus(id, listName, status);
        ListItemEntity listItemEntity = listItemService.find(id, listName);
        REQUIRE(*(*listItemEntity.status()).getCommandName() == "paused");

        REQUIRE_THROWS(listItemService.setStatus(id, listName, status));

        std::string statusValue2 = "to-do";
        const std::string* status2 = &statusValue2;
        listItemService.setStatus(id, listName, status2);
        ListItemEntity listItemEntity2 = listItemService.find(id, listName);
        REQUIRE(*(*listItemEntity2.status()).getCommandName() == "to-do");
    }

    SECTION("min/max priority")
    {
        REQUIRE(priorityService.isMax("critical"));
        REQUIRE_FALSE(priorityService.isMax("high"));
        REQUIRE_FALSE(priorityService.isMax("low"));

        REQUIRE(priorityService.isMin("low"));
        REQUIRE_FALSE(priorityService.isMin("critical"));
        REQUIRE_FALSE(priorityService.isMin("medium"));
    }

    SECTION("empty")
    {
        listItemService.archiveAll(listName);
        std::vector<ListItemEntity> listItems = listItemService.get(listName);
        REQUIRE(listItems.empty());

        std::vector<ListItemEntity> listArchiveItems = listItemService.get(listNameArchive);
        REQUIRE(listArchiveItems.size() == 2);

        installation.wipe();
        installation.make();
    }

    SECTION("archiveFinishedItems")
    {
        std::string priorityValue = "low";
        const std::string* priority = &priorityValue;
        std::string statusValue = "to-do";
        const std::string* status = &statusValue;
        std::string value = "test 3";
        std::string id = listItemService.add(listName, value, priority, status);
        listItemService.editStatus(id, listName, new int(StatusService::COMPLETED));

        std::vector<ListItemEntity> listItems = listItemService.get(listName);
        REQUIRE(listItems.size() == 3);
        REQUIRE(*listItems[0].getId() == "aaaa");
        REQUIRE(*listItems[1].getId() == "bbbb");
        REQUIRE(*listItems[2].getId() == id);

        listItemService.archiveFinishedItems(listName);
        std::vector<ListItemEntity> listItems2 = listItemService.get(listName);
        REQUIRE(listItems2.size() == 2);
        REQUIRE(*listItems2[0].getId() == "aaaa");
        REQUIRE(*listItems2[1].getId() == "bbbb");

        std::vector<ListItemEntity> listArchiveItems = listItemService.get(listNameArchive);
        REQUIRE(listArchiveItems.size() == 1);

        listItemService.restore(id, listName);
        std::vector<ListItemEntity> listItems3 = listItemService.get(listName);
        REQUIRE(listItems3.size() == 3);
        REQUIRE(*listItems3[0].getId() == "aaaa");
        REQUIRE(*listItems3[1].getId() == "bbbb");
        REQUIRE(*listItems3[2].getId() == id);

        std::vector<ListItemEntity> listArchiveItems2 = listItemService.get(listNameArchive);
        REQUIRE(listArchiveItems2.size() == 0);

        listItemService.remove(id, listName);

        installation.wipe();
        installation.make();
    }

    SECTION("Archive and restore item")
    {
        std::string id = "aaaa";
        std::vector<ListItemEntity> listItems = listItemService.get(listName);
        REQUIRE(listItems.size() == 2);
        REQUIRE(*listItems[0].getId() == "aaaa");
        REQUIRE(*listItems[1].getId() == "bbbb");

        listItemService.archive(id, listName);

        std::vector<ListItemEntity> listItems2 = listItemService.get(listName);
        REQUIRE(listItems2.size() == 1);

        listItemService.restore(id, listName);

        std::vector<ListItemEntity> listItems3 = listItemService.get(listName);
        REQUIRE(listItems3.size() == 2);

        installation.wipe();
        installation.make();
    }

    SECTION("Move item to another list")
    {
        std::string id = "aaaa";
        std::vector<ListItemEntity> listItems = listItemService.get(tempListName);
        REQUIRE(listItems.size() == 2);
        REQUIRE(*listItems[0].getId() == "aaaa");
        REQUIRE(*listItems[1].getId() == "bbbb");

        REQUIRE_THROWS(listItemService.move(id, tempListName, tempListName));

        listItemService.move(id, tempListName, tempListName2);
        std::vector<ListItemEntity> listItems2 = listItemService.get(tempListName2);
        REQUIRE(listItems2.size() == 3);
        REQUIRE(*listItemService.find(id, tempListName2).getId() == "aaaa");
        std::vector<ListItemEntity> listItems3 = listItemService.get(tempListName);
        REQUIRE(listItems3.size() == 1);
        REQUIRE(*listItems3[0].getId() == "bbbb");

        installation.wipe();
        installation.make();
    }

    SECTION("Copy item to another list")
    {
        std::string id = "aaaa";
        std::vector<ListItemEntity> listItems = listItemService.get(tempListName);
        REQUIRE(listItems.size() == 2);
        REQUIRE(*listItems[0].getId() == "aaaa");
        REQUIRE(*listItems[1].getId() == "bbbb");

        listItemService.copy(id, tempListName, tempListName2);
        std::vector<ListItemEntity> listItems2 = listItemService.get(tempListName2);
        REQUIRE(listItems2.size() == 3);
        REQUIRE(*listItemService.find(id, tempListName2).getId() == "aaaa");

        std::vector<ListItemEntity> listItems3 = listItemService.get(tempListName);
        REQUIRE(listItems3.size() == 2);
        REQUIRE(*listItems3[0].getId() == "aaaa");
        REQUIRE(*listItems3[1].getId() == "bbbb");

        // Can copy/move an item that already exist in list (it will delete previous -- confirmation asked in
        // Controller)
        REQUIRE_NOTHROW(listItemService.copy(id, tempListName, tempListName2));
        REQUIRE_NOTHROW(listItemService.move(id, tempListName, tempListName2));

        installation.wipe();
        installation.make();
    }

    SECTION("Edit deadline of an item")
    {
        std::string id =
            listItemService.add(tempListName, "test item", new std::string("high"), new std::string("to-do"));

        std::string deadline0 = "2024.12.31";
        time_t dueAt0 = DateHelpers::relativeDateToTimestamp(deadline0);
        listItemService.editDeadline(id, tempListName, dueAt0);
        ListItemEntity listItem0 = listItemService.find(id, tempListName);
        std::string updatedDeadline0 = DateHelpers::formatTimestamp(*listItem0.getDueAt(), "ymd", ".");
        REQUIRE(updatedDeadline0 == deadline0);

        std::string deadline = "2024-12-31";
        time_t dueAt = DateHelpers::relativeDateToTimestamp(deadline);
        listItemService.editDeadline(id, tempListName, dueAt);
        ListItemEntity listItem = listItemService.find(id, tempListName);
        std::string updatedDeadline = DateHelpers::formatTimestamp(*listItem.getDueAt(), "ymd", "-");
        REQUIRE(updatedDeadline == deadline);

        std::string deadline2 = "2024-05-15";
        std::string deadline2Request = "05-15";
        time_t dueAt2 = DateHelpers::relativeDateToTimestamp(deadline2);
        listItemService.editDeadline(id, tempListName, dueAt2);
        ListItemEntity listItem2 = listItemService.find(id, tempListName);
        std::string updatedDeadline2 = DateHelpers::formatTimestamp(*listItem2.getDueAt(), "ymd", "-");
        REQUIRE(updatedDeadline2 == deadline2);

        std::string deadline3 = "2024-05-15";
        std::string deadline3Request = "15";
        time_t dueAt3 = DateHelpers::relativeDateToTimestamp(deadline3);
        listItemService.editDeadline(id, tempListName, dueAt3);
        ListItemEntity listItem3 = listItemService.find(id, tempListName);
        std::string updatedDeadline3 = DateHelpers::formatTimestamp(*listItem3.getDueAt(), "ymd", "-");
        REQUIRE(updatedDeadline3 == deadline3);

        listItemService.editDeadline(id, tempListName);
        ListItemEntity listItem4 = listItemService.find(id, tempListName);
        REQUIRE(*listItem4.getDueAt() == 0);
    }

    SECTION("Count()")
    {
        std::vector<ListItemEntity> listItems = listItemService.get(tempListName);
        REQUIRE(listItems.size() == listItemService.count(tempListName));
    }

    SECTION("countWithStatus()")
    {
        REQUIRE(1 == listItemService.countWithStatus(tempListName, { 0 }));
        REQUIRE(0 == listItemService.countWithStatus(tempListName, { 4 }));
        REQUIRE(1 == listItemService.countWithStatus(tempListName, { 1, 4 }));
        REQUIRE_THROWS(listItemService.countWithStatus(tempListName, { 10 }));
    }

    SECTION("countWithPriority()")
    {
        REQUIRE(1 == listItemService.countWithPriority(tempListName, { 1 }));
        REQUIRE(0 == listItemService.countWithPriority(tempListName, { 0 }));
        REQUIRE(0 == listItemService.countWithPriority(tempListName, { 0, 3 }));
        REQUIRE(2 == listItemService.countWithPriority(tempListName, { 1, 2 }));
        REQUIRE_THROWS(listItemService.countWithPriority(tempListName, { 10 }));
    }
}
