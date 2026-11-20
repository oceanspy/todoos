#include "../../List/ListItemService.h"
#include "../../FileDataStorageRepositories/DescriptionRepository.h"
#include "../../FileDataStorageRepositories/ListRepository.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemId.h"
#include "../../List/ListService.h"
#include "../../Serializers/ConfSerializer.h"
#include "../../Serializers/JsonSerializer.h"
#include "../Mock/MockAppInitialization.h"
#include "../Mock/MockAppInstallation.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <filesystem>
#include <fstream>

// {"aaaa", "test 1", "3", "0", "1712487259", "1712487259", "0", "0"}
// {"bbbb", "test 2", "1", "1", "1712487272", "1712487272", "1712487259", "0"}

TEST_CASE("ListItemServiceTest", "[ListItemService]")
{
    // Create mock objects
    IOService ioService("cli");
    ConfSerializer confService = ConfSerializer(ioService);
    JsonSerializer jsonService = JsonSerializer(ioService);
    std::unique_ptr<DataSerializerInterface> fileDataStorageServicePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> fileDataConfigStorageServicePtr =
        std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_ListItemServiceTest");
    MockAppInstallation installation(ioService, jsonService, confService, init);
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
    DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());

    ListItemService listItemService(
        ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
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
        REQUIRE(*(*listItemEntity.status()).getCommandName() == "queued");
        REQUIRE(*(*listItemEntity.priority()).getName() == "high");
        REQUIRE(*listItemEntity.getCreatedAt() == 1712487259);
        REQUIRE(*listItemEntity.getUpdatedAt() == 1712487259);
    }

    SECTION("add")
    {
        std::string priorityValue = "high";
        const std::string* priority = &priorityValue;
        std::string statusValue = "queued";
        const std::string* status = &statusValue;
        std::string value = "test 1";

        std::string id = listItemService.add(listName, value, priority, status);
        ListItemEntity listItemEntity = listItemService.find(id, listName);
        REQUIRE(*listItemEntity.getId() == id);

        std::string id2 = listItemService.add(listName, value);
        ListItemEntity listItemEntity2 = listItemService.find(id2, listName);
        REQUIRE(*listItemEntity2.getId() == id2);
        REQUIRE(*(*listItemEntity2.priority()).getName() == "urgent");
        REQUIRE(*(*listItemEntity2.status()).getCommandName() == "queued");
    }

    SECTION("add with a chosen id: used when free, generated when taken, refused when malformed")
    {
        std::string value = "chosen";
        REQUIRE(listItemService.add(listName, value, nullptr, nullptr, 0, "", "SRV1") == "SRV1");
        REQUIRE(*listItemService.find("SRV1", listName).getValue() == "chosen");

        // taken: a generated id, the existing item is untouched
        const std::string generated = listItemService.add(listName, value, nullptr, nullptr, 0, "", "aaaa");
        REQUIRE(generated != "aaaa");
        REQUIRE(ListItemId::isValidActive(generated));
        REQUIRE(*listItemService.find("aaaa", listName).getValue() != "chosen");

        const size_t count = listItemService.get(listName).size();
        for (const std::string bad : { "ab", "abcde", "ab-c", "ab c" }) {
            REQUIRE_THROWS(listItemService.add(listName, value, nullptr, nullptr, 0, "", bad));
        }
        REQUIRE_THROWS(listItemService.add(listNameArchive, value, nullptr, nullptr, 0, "", "ARC1"));
        REQUIRE(listItemService.get(listName).size() == count);
    }

    SECTION("add too long")
    {
        std::string priorityValue = "high";
        const std::string* priority = &priorityValue;
        std::string statusValue = "queued";
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
        std::string statusValue = "queued";
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
        std::string statusValue = "queued";
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
        REQUIRE(*(*listItemEntity.status()).getCommandName() == "queued");
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
        const int statusValue = StatusService::QUEUED;
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
        REQUIRE(*(*listItemEntity.status()).getCommandName() == "queued");
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

        REQUIRE_THROWS_AS(listItemService.find("aaaa", listName), ListItemNotFoundException);
        std::vector<ListItemEntity> archived = listItemService.get(listNameArchive);
        REQUIRE(archived.size() == 1);
        REQUIRE(ListItemId::baseOf(*archived[0].getId()) == "aaaa");
    }

    SECTION("add & remove")
    {
        std::string priorityValue = "low";
        const std::string* priority = &priorityValue;
        std::string statusValue = "queued";
        const std::string* status = &statusValue;
        std::string value = "test 3";
        std::string id = listItemService.add(tempListName, value, priority, status);

        ListItemEntity listItemEntity = listItemService.find(id, tempListName);
        REQUIRE(*listItemEntity.getId() == id);
        REQUIRE(*listItemEntity.getValue() == "test 3");
        REQUIRE(*(*listItemEntity.priority()).getName() == "low");
        REQUIRE(*(*listItemEntity.status()).getCommandName() == "queued");

        std::vector<ListItemEntity> listItems = listItemService.get(tempListName);
        REQUIRE(listItems.size() == 3);
        REQUIRE(*listItems[0].getId() == "aaaa");
        REQUIRE(*listItems[1].getId() == "bbbb");
        REQUIRE(*listItems[2].getId() == id);

        std::string deletedId = listItemService.softDelete(id, tempListName);
        REQUIRE(deletedId.length() == 5);
        REQUIRE(ListItemId::baseOf(deletedId) == id);

        std::vector<ListItemEntity> listDelItems = listItemService.get(tempListNameDelete);
        REQUIRE(listDelItems.size() == 1);
        REQUIRE(*listDelItems[0].getId() == deletedId);

        std::vector<ListItemEntity> listItems2 = listItemService.get(tempListName);
        REQUIRE(listItems2.size() == 2);
        REQUIRE(*listItems2[0].getId() == "aaaa");
        REQUIRE(*listItems2[1].getId() == "bbbb");

        REQUIRE(listItemService.restore(deletedId, tempListName) == id);
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
        std::string statusValue = "queued";
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
        REQUIRE(*(*listItemEntity.status()).getCommandName() == "queued");
        REQUIRE(*listItemEntity.getCreatedAt() == 1712487259);

        std::string priorityValue = "high";
        const std::string* priority = &priorityValue;
        std::string statusValue = "queued";
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
        REQUIRE(*(*listItemEntity.status()).getCommandName() == "queued");
        REQUIRE(*listItemEntity.getCreatedAt() == 1712487259);

        std::string priorityValue = "high";
        const std::string* priority = &priorityValue;
        std::string statusValue = "queued";
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

        std::string statusValue2 = "queued";
        const std::string* status2 = &statusValue2;
        listItemService.setStatus(id, listName, status2);
        ListItemEntity listItemEntity2 = listItemService.find(id, listName);
        REQUIRE(*(*listItemEntity2.status()).getCommandName() == "queued");
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
        std::string statusValue = "queued";
        const std::string* status = &statusValue;
        std::string value = "test 3";
        std::string id = listItemService.add(listName, value, priority, status);
        listItemService.editStatus(id, listName, new int(StatusService::COMPLETED));

        std::vector<ListItemEntity> listItems = listItemService.get(listName);
        REQUIRE(listItems.size() == 3);
        REQUIRE(*listItems[0].getId() == "aaaa");
        REQUIRE(*listItems[1].getId() == "bbbb");
        REQUIRE(*listItems[2].getId() == id);

        listItemService.archiveFinishedItems(listName, false);
        std::vector<ListItemEntity> listItems2 = listItemService.get(listName);
        REQUIRE(listItems2.size() == 2);
        REQUIRE(*listItems2[0].getId() == "aaaa");
        REQUIRE(*listItems2[1].getId() == "bbbb");

        std::vector<ListItemEntity> listArchiveItems = listItemService.get(listNameArchive);
        REQUIRE(listArchiveItems.size() == 1);

        REQUIRE(listItemService.restore(*listArchiveItems[0].getId(), listName) == id);
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

    SECTION("archiveFinishedItems skips described items unless withDescribedItems is true")
    {
        std::string priorityValue = "low";
        const std::string* priority = &priorityValue;
        std::string statusValue = "queued";
        const std::string* status = &statusValue;
        std::string id = listItemService.add(listName, "described finished item", priority, status);
        listItemService.editStatus(id, listName, new int(StatusService::COMPLETED));

        std::filesystem::path descPath = descriptionRepository.getFilePath(id, listName);
        std::filesystem::create_directories(descPath.parent_path());
        std::ofstream(descPath) << "Some description.";

        // Without -d: described completed item must NOT be archived
        listItemService.archiveFinishedItems(listName, false);

        std::vector<ListItemEntity> listItems = listItemService.get(listName);
        auto found = std::find_if(listItems.begin(), listItems.end(), [&id](const ListItemEntity& i) {
            return *i.getId() == id;
        });
        REQUIRE(found != listItems.end());
        REQUIRE(listItemService.get(listNameArchive).empty());

        // With -d: described completed item SHOULD be archived
        listItemService.archiveFinishedItems(listName, true);

        std::vector<ListItemEntity> listItems2 = listItemService.get(listName);
        auto found2 = std::find_if(listItems2.begin(), listItems2.end(), [&id](const ListItemEntity& i) {
            return *i.getId() == id;
        });
        REQUIRE(found2 == listItems2.end());

        std::vector<ListItemEntity> archiveItems = listItemService.get(listNameArchive);
        REQUIRE(archiveItems.size() == 1);
        REQUIRE(ListItemId::baseOf(*archiveItems[0].getId()) == id);

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

        std::string archivedId = listItemService.archive(id, listName);

        std::vector<ListItemEntity> listItems2 = listItemService.get(listName);
        REQUIRE(listItems2.size() == 1);

        REQUIRE(listItemService.restore(archivedId, listName) == id);

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
            listItemService.add(tempListName, "test item", new std::string("high"), new std::string("queued"));

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

    SECTION("duplicate")
    {
        std::string id = "aaaa";
        std::vector<ListItemEntity> listItemsBefore = listItemService.get(tempListName);
        REQUIRE(listItemsBefore.size() == 2);

        listItemService.duplicate(id, tempListName);

        std::vector<ListItemEntity> listItemsAfter = listItemService.get(tempListName);
        REQUIRE(listItemsAfter.size() == 3);

        // The duplicated item should have a different ID but same value
        bool foundOriginal = false;
        bool foundDuplicate = false;
        for (auto& item : listItemsAfter) {
            if (*item.getId() == "aaaa") {
                foundOriginal = true;
            } else if (*item.getValue() == "test 1" && *item.getId() != "aaaa") {
                foundDuplicate = true;
            }
        }
        REQUIRE(foundOriginal);
        REQUIRE(foundDuplicate);

        installation.wipe();
        installation.make();
    }

    SECTION("reset")
    {
        std::string id = "aaaa";

        // First change status to something else
        const int newStatusValue = StatusService::STARTED;
        const int* newStatus = &newStatusValue;
        listItemService.editStatus(id, listName, newStatus);
        ListItemEntity listItemEntity = listItemService.find(id, listName);
        REQUIRE(*(*listItemEntity.status()).getCommandName() == "started");

        // Reset should bring it back to to-do
        listItemService.reset(id, listName);
        ListItemEntity resetItem = listItemService.find(id, listName);
        REQUIRE(*(*resetItem.status()).getCommandName() == "queued");
    }

    SECTION("find non-existent item throws")
    {
        REQUIRE_THROWS_AS(listItemService.find("zzzz", listName), ListItemNotFoundException);
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

    SECTION("countCreatedBetween()")
    {
        // Both items were created at 1712487259 and 1712487272
        REQUIRE(2 == listItemService.countCreatedBetween(tempListName, 1712487250, 1712487280));
        REQUIRE(1 == listItemService.countCreatedBetween(tempListName, 1712487260, 1712487280));
        REQUIRE(0 == listItemService.countCreatedBetween(tempListName, 1712487280, 1712487290));
    }

    SECTION("countClosedBetween()")
    {
        // No items are closed initially
        REQUIRE(0 == listItemService.countClosedBetween(tempListName, 0, time(nullptr) + 100000));
    }

    SECTION("makeId returns a string of correct length")
    {
        std::string id = listItemService.makeId(tempListName);
        REQUIRE(id.length() == static_cast<size_t>(ListItemId::getIdLength()));
    }

    SECTION("isIdAvailable")
    {
        REQUIRE(listItemService.isIdAvailable("zzzz", tempListName) == true);
        REQUIRE(listItemService.isIdAvailable("aaaa", tempListName) == false);
    }

    SECTION("get sets hasDescription to false when no description files exist")
    {
        std::vector<ListItemEntity> items = listItemService.get(tempListName);
        for (const ListItemEntity& item : items) {
            REQUIRE_FALSE(*item.hasDescription());
        }
    }

    SECTION("get sets hasDescription to true for items with a description file")
    {
        std::filesystem::path descPath = descriptionRepository.getFilePath("aaaa", tempListName);
        std::filesystem::create_directories(descPath.parent_path());
        std::ofstream(descPath) << "Some description.";

        std::vector<ListItemEntity> items = listItemService.get(tempListName);
        auto aaaa =
            std::find_if(items.begin(), items.end(), [](const ListItemEntity& i) { return *i.getId() == "aaaa"; });
        auto bbbb =
            std::find_if(items.begin(), items.end(), [](const ListItemEntity& i) { return *i.getId() == "bbbb"; });

        REQUIRE(aaaa != items.end());
        REQUIRE(bbbb != items.end());
        REQUIRE(*aaaa->hasDescription());
        REQUIRE_FALSE(*bbbb->hasDescription());

        installation.wipe();
        installation.make();
    }

    // ---- 5-char ids for archived / deleted items --------------------------------------------

    auto writeDescription = [&descriptionRepository](const std::string& id, ListName& ln, const std::string& text) {
        std::filesystem::path descPath = descriptionRepository.getFilePath(id, ln);
        std::filesystem::create_directories(descPath.parent_path());
        std::ofstream(descPath) << text;
    };

    // Puts a copy of `bbbb` (from the active temp list) into any list under `variantId`
    auto seedVariantItem = [&](ListName& variantListName, const std::string& variantId) {
        ListItemEntity seeded = listItemRepository.find("bbbb", tempListName);
        seeded.setId(variantId);
        listItemRepository.create(seeded, variantListName);
    };

    SECTION("moveToVariant: archive gives base + one [a-z] suffix and removes the item from the active list")
    {
        std::string newId = listItemService.moveToVariant("aaaa", tempListName, tempListNameArchive);

        REQUIRE(newId.length() == 5);
        REQUIRE(newId.substr(0, 4) == "aaaa");
        REQUIRE(newId[4] >= 'a');
        REQUIRE(newId[4] <= 'z');
        REQUIRE_THROWS_AS(listItemService.find("aaaa", tempListName), ListItemNotFoundException);
        REQUIRE(*listItemService.find(newId, tempListNameArchive).getValue() ==
                *listItemRepository.find(newId, tempListNameArchive).getValue());
        REQUIRE(listItemService.get(tempListNameArchive).size() == 1);
        REQUIRE(listItemService.get(tempListNameDelete).empty());
    }

    SECTION("moveToVariant: delete gives base + one [a-z] suffix")
    {
        std::string newId = listItemService.moveToVariant("bbbb", tempListName, tempListNameDelete);

        REQUIRE(newId.length() == 5);
        REQUIRE(newId.substr(0, 4) == "bbbb");
        REQUIRE_NOTHROW(listItemService.find(newId, tempListNameDelete));
        REQUIRE(listItemService.get(tempListName).size() == 1);
    }

    SECTION("moveToVariant: item fields are preserved")
    {
        ListItemEntity before = listItemService.find("aaaa", tempListName);
        std::string newId = listItemService.moveToVariant("aaaa", tempListName, tempListNameArchive);
        ListItemEntity after = listItemService.find(newId, tempListNameArchive);

        REQUIRE(*after.getValue() == *before.getValue());
        REQUIRE(*(*after.priority()).getName() == *(*before.priority()).getName());
        REQUIRE(*(*after.status()).getCommandName() == *(*before.status()).getCommandName());
        REQUIRE(*after.getCreatedAt() == *before.getCreatedAt());
        REQUIRE(*after.getUpdatedAt() == *before.getUpdatedAt());
    }

    SECTION("moveToVariant: description follows the new id")
    {
        writeDescription("aaaa", tempListName, "my description");

        std::string newId = listItemService.moveToVariant("aaaa", tempListName, tempListNameArchive);

        REQUIRE_FALSE(descriptionRepository.exists("aaaa", tempListName));
        REQUIRE(descriptionRepository.exists(newId, tempListName));
        REQUIRE(descriptionRepository.load(newId, tempListName) == "my description");
        REQUIRE(*listItemService.find(newId, tempListNameArchive).hasDescription());
    }

    SECTION("moveToVariant: item without description creates no description file")
    {
        std::string newId = listItemService.moveToVariant("aaaa", tempListName, tempListNameArchive);
        REQUIRE_FALSE(descriptionRepository.exists(newId, tempListName));
    }

    SECTION("moveToVariant: same base archived twice gets two distinct ids")
    {
        std::string firstId = listItemService.moveToVariant("aaaa", tempListName, tempListNameArchive);

        // The 4-char id is free again in the active list
        REQUIRE(listItemService.isIdAvailable("aaaa", tempListName));
        seedVariantItem(tempListName, "aaaa");

        std::string secondId = listItemService.moveToVariant("aaaa", tempListName, tempListNameArchive);

        REQUIRE(firstId != secondId);
        REQUIRE(secondId.substr(0, 4) == "aaaa");
        REQUIRE(listItemService.get(tempListNameArchive).size() == 2);
    }

    SECTION("moveToVariant: suffix is unique across archive AND delete")
    {
        // Every suffix but 'q' is taken: half in archive, half in delete
        for (char c = 'a'; c <= 'z'; c++) {
            if (c == 'q') {
                continue;
            }
            seedVariantItem(c < 'n' ? tempListNameArchive : tempListNameDelete, std::string("aaaa") + c);
        }

        REQUIRE(listItemService.moveToVariant("aaaa", tempListName, tempListNameArchive) == "aaaaq");
    }

    SECTION("moveToVariant: all 26 suffixes taken throws and leaves the item untouched")
    {
        writeDescription("aaaa", tempListName, "kept");
        for (char c = 'a'; c <= 'z'; c++) {
            seedVariantItem(tempListNameArchive, std::string("aaaa") + c);
        }

        REQUIRE_THROWS_AS(listItemService.moveToVariant("aaaa", tempListName, tempListNameArchive), std::runtime_error);
        REQUIRE_NOTHROW(listItemService.find("aaaa", tempListName));
        REQUIRE(listItemService.get(tempListNameArchive).size() == 26);
        REQUIRE(descriptionRepository.load("aaaa", tempListName) == "kept");
    }

    SECTION("moveToVariant: archive -> delete keeps the 5-char id and its description")
    {
        std::string archivedId = listItemService.moveToVariant("aaaa", tempListName, tempListNameArchive);
        writeDescription(archivedId, tempListName, "desc");

        std::string deletedId = listItemService.moveToVariant(archivedId, tempListNameArchive, tempListNameDelete);

        REQUIRE(deletedId == archivedId);
        REQUIRE(listItemService.get(tempListNameArchive).empty());
        REQUIRE_NOTHROW(listItemService.find(deletedId, tempListNameDelete));
        REQUIRE(descriptionRepository.load(deletedId, tempListName) == "desc");
    }

    SECTION("moveToVariant: rejects another list, the default variant and the same variant")
    {
        REQUIRE_THROWS_AS(listItemService.moveToVariant("aaaa", tempListName, tempListName2Archive),
                          std::invalid_argument);
        REQUIRE_THROWS_AS(listItemService.moveToVariant("aaaa", tempListName, tempListName2Delete),
                          std::invalid_argument);
        REQUIRE_THROWS_AS(listItemService.moveToVariant("aaaa", tempListName, tempListName), std::invalid_argument);

        std::string archivedId = listItemService.moveToVariant("aaaa", tempListName, tempListNameArchive);
        REQUIRE_THROWS_AS(listItemService.moveToVariant(archivedId, tempListNameArchive, tempListNameArchive),
                          std::invalid_argument);
        REQUIRE_THROWS_AS(listItemService.moveToVariant(archivedId, tempListNameArchive, tempListName),
                          std::invalid_argument);

        REQUIRE(listItemService.get(tempListNameArchive).size() == 1);
        REQUIRE(listItemService.get(tempListName2Archive).empty());
        REQUIRE(listItemService.get(tempListName2Delete).empty());
        REQUIRE(listItemService.get(tempListName).size() == 1);
    }

    SECTION("moveToVariant: unknown id throws and changes nothing")
    {
        REQUIRE_THROWS_AS(listItemService.moveToVariant("zzzz", tempListName, tempListNameArchive),
                          ListItemNotFoundException);
        REQUIRE(listItemService.get(tempListName).size() == 2);
        REQUIRE(listItemService.get(tempListNameArchive).empty());
    }

    SECTION("archive, softDelete, archiveAll and auto-archive all produce 5-char ids")
    {
        REQUIRE(listItemService.archive("aaaa", tempListName).length() == 5);
        REQUIRE(listItemService.softDelete("bbbb", tempListName).length() == 5);

        listItemService.archiveAll(listName);
        for (const ListItemEntity& item : listItemService.get(listNameArchive)) {
            REQUIRE((*item.getId()).length() == 5);
        }

        configService.edit("archiveWhenCompleted", "true");
        std::string id = listItemService.add(tempListName2, "auto archived");
        const int completed = StatusService::COMPLETED;
        listItemService.editStatus(id, tempListName2, &completed);
        std::vector<ListItemEntity> archived = listItemService.get(tempListName2Archive);
        REQUIRE(archived.size() == 1);
        REQUIRE(*archived[0].getId() != id);
        REQUIRE(ListItemId::baseOf(*archived[0].getId()) == id);
        configService.edit("archiveWhenCompleted", "false");
    }

    SECTION("auto-archive does not fire when editing an archived item")
    {
        configService.edit("archiveWhenCompleted", "true");
        std::string archivedId = listItemService.archive("aaaa", tempListName);
        const int completed = StatusService::COMPLETED;

        REQUIRE_NOTHROW(listItemService.editStatus(archivedId, tempListNameArchive, &completed));
        REQUIRE(listItemService.get(tempListNameArchive).size() == 1);
        configService.edit("archiveWhenCompleted", "false");
    }

    SECTION("setStatus sets closedAt on a closed status and clears it otherwise")
    {
        std::string completedName = "completed";
        std::string queuedName = "queued";

        listItemService.setStatus("aaaa", tempListName, &completedName);
        REQUIRE(*listItemService.find("aaaa", tempListName).getClosedAt() > 0);

        listItemService.setStatus("aaaa", tempListName, &queuedName);
        REQUIRE(*listItemService.find("aaaa", tempListName).getClosedAt() == 0);
    }

    SECTION("setStatus auto-archives on a closed status when archiveWhenCompleted is on")
    {
        std::string cancelledName = "cancelled";

        configService.edit("archiveWhenCompleted", "true");
        listItemService.setStatus("aaaa", tempListName, &cancelledName);
        std::vector<ListItemEntity> archived = listItemService.get(tempListNameArchive);
        REQUIRE(archived.size() == 1);
        REQUIRE(ListItemId::baseOf(*archived[0].getId()) == "aaaa");
        configService.edit("archiveWhenCompleted", "false");

        listItemService.setStatus("bbbb", tempListName, &cancelledName);
        REQUIRE(listItemService.get(tempListNameArchive).size() == 1);
    }

    SECTION("isIdAvailable only looks at the active list")
    {
        listItemService.archive("aaaa", tempListName);
        listItemService.softDelete("bbbb", tempListName);

        REQUIRE(listItemService.isIdAvailable("aaaa", tempListName));
        REQUIRE(listItemService.isIdAvailable("bbbb", tempListName));
    }

    SECTION("restore: original 4-char id is given back when free, with its description")
    {
        writeDescription("aaaa", tempListName, "restored desc");
        std::string archivedId = listItemService.archive("aaaa", tempListName);

        REQUIRE(listItemService.restore(archivedId, tempListName) == "aaaa");
        REQUIRE_NOTHROW(listItemService.find("aaaa", tempListName));
        REQUIRE(listItemService.get(tempListNameArchive).empty());
        REQUIRE(descriptionRepository.load("aaaa", tempListName) == "restored desc");
        REQUIRE_FALSE(descriptionRepository.exists(archivedId, tempListName));
    }

    SECTION("restore: a new id is generated when the base is taken, description follows")
    {
        writeDescription("aaaa", tempListName, "old item desc");
        std::string archivedId = listItemService.archive("aaaa", tempListName);
        seedVariantItem(tempListName, "aaaa"); // a new active item now owns "aaaa"
        std::string archivedText = *listItemService.find(archivedId, tempListNameArchive).getValue();

        std::string restoredId = listItemService.restore(archivedId, tempListName);

        REQUIRE(restoredId != "aaaa");
        REQUIRE(restoredId.length() == 4);
        REQUIRE(*listItemService.find(restoredId, tempListName).getValue() == archivedText);
        REQUIRE(listItemService.get(tempListName).size() == 3);
        REQUIRE(descriptionRepository.load(restoredId, tempListName) == "old item desc");
        REQUIRE_FALSE(descriptionRepository.exists("aaaa", tempListName));
    }

    SECTION("restore: works from the delete list")
    {
        std::string deletedId = listItemService.softDelete("bbbb", tempListName);
        REQUIRE(listItemService.restore(deletedId, tempListName) == "bbbb");
        REQUIRE(listItemService.get(tempListNameDelete).empty());
    }

    SECTION("restore: the 4-char base alone is not an id")
    {
        listItemService.archive("aaaa", tempListName);
        REQUIRE_THROWS_AS(listItemService.restore("aaaa", tempListName), std::invalid_argument);
        REQUIRE(listItemService.get(tempListNameArchive).size() == 1);
    }

    SECTION("hasLegacyVariantIds: false on migrated / empty lists, true with a 4-char id in archive or delete")
    {
        REQUIRE_FALSE(listItemService.hasLegacyVariantIds(tempListName));

        listItemService.archive("aaaa", tempListName);
        listItemService.softDelete(listItemService.add(tempListName, "to delete"), tempListName);
        REQUIRE_FALSE(listItemService.hasLegacyVariantIds(tempListName));

        seedVariantItem(tempListNameArchive, "cccc");
        REQUIRE(listItemService.hasLegacyVariantIds(tempListName));
    }

    SECTION("hasLegacyVariantIds: detects a 4-char id in the delete list")
    {
        seedVariantItem(tempListNameDelete, "cccc");
        REQUIRE(listItemService.hasLegacyVariantIds(tempListName));
        REQUIRE_FALSE(listItemService.hasLegacyVariantIds(tempListName2));
    }

    SECTION("hasLegacyRowLayout: detects 8-column rows in the list, its archive or its delete list")
    {
        REQUIRE_FALSE(listItemService.hasLegacyRowLayout(tempListName));

        jsonService.load(configService.getListArchiveFilePathFromFilePath(configService.getListFilePath("tempListName")));
        jsonService.write({ { "eeeea", "legacy", "high", "queued", "0", "0", "1", "1" } });
        listItemRepository.resetCache("");
        REQUIRE(listItemService.hasLegacyRowLayout(tempListName));
        REQUIRE_FALSE(listItemService.hasLegacyRowLayout(tempListName2));
    }

    SECTION("restore: unknown id throws")
    {
        REQUIRE_THROWS_AS(listItemService.restore("zzzzz", tempListName), std::invalid_argument);
    }

    SECTION("move / copy / duplicate are rejected from archive and delete lists")
    {
        std::string archivedId = listItemService.archive("aaaa", tempListName);
        std::string deletedId = listItemService.softDelete("bbbb", tempListName);

        REQUIRE_THROWS_AS(listItemService.move(archivedId, tempListNameArchive, tempListName2), std::invalid_argument);
        REQUIRE_THROWS_AS(listItemService.copy(archivedId, tempListNameArchive, tempListName2), std::invalid_argument);
        REQUIRE_THROWS_AS(listItemService.duplicate(archivedId, tempListNameArchive), std::invalid_argument);
        REQUIRE_THROWS_AS(listItemService.move(deletedId, tempListNameDelete, tempListName2), std::invalid_argument);
        REQUIRE_THROWS_AS(listItemService.copy(deletedId, tempListNameDelete, tempListName2), std::invalid_argument);
        REQUIRE_THROWS_AS(listItemService.duplicate(deletedId, tempListNameDelete), std::invalid_argument);

        REQUIRE(listItemService.get(tempListName2).size() == 2);
        REQUIRE(listItemService.get(tempListNameArchive).size() == 1);
        REQUIRE(listItemService.get(tempListNameDelete).size() == 1);
    }

    // ---- reassign ----------------------------------------------------------------------------

    SECTION("reassign: the item gets the new id and keeps its place in the file")
    {
        listItemService.reassign("aaaa", "SRV1", tempListName);

        std::vector<ListItemEntity> rows = listItemRepository.get(tempListName);
        REQUIRE(rows.size() == 2);
        REQUIRE(*rows[0].getId() == "SRV1");
        REQUIRE(*rows[0].getValue() == "test 1");
        REQUIRE(*rows[1].getId() == "bbbb");
        REQUIRE_THROWS_AS(listItemService.find("aaaa", tempListName), ListItemNotFoundException);
    }

    SECTION("reassign: refuses a taken id, a bad format, the same id, an unknown or archived item")
    {
        REQUIRE_THROWS_AS(listItemService.reassign("aaaa", "bbbb", tempListName), std::invalid_argument);
        REQUIRE_THROWS_AS(listItemService.reassign("aaaa", "ab", tempListName), std::invalid_argument);
        REQUIRE_THROWS_AS(listItemService.reassign("aaaa", "ab-c", tempListName), std::invalid_argument);
        REQUIRE_THROWS_AS(listItemService.reassign("aaaa", "abcde", tempListName), std::invalid_argument);
        REQUIRE_THROWS_AS(listItemService.reassign("aaaa", "aaaa", tempListName), std::invalid_argument);
        REQUIRE_THROWS(listItemService.reassign("zzzz", "SRV1", tempListName));

        std::string archivedId = listItemService.archive("bbbb", tempListName);
        REQUIRE_THROWS_AS(listItemService.reassign(archivedId, "SRV1", tempListNameArchive), std::invalid_argument);

        // nothing changed
        REQUIRE(*listItemService.find("aaaa", tempListName).getId() == "aaaa");
    }

    SECTION("reassign: the id is case-sensitive, like every id")
    {
        listItemService.reassign("aaaa", "AAAA", tempListName);
        REQUIRE(*listItemService.find("AAAA", tempListName).getValue() == "test 1");
    }

    SECTION("reassign: the description follows")
    {
        writeDescription("aaaa", tempListName, "notes");
        listItemService.reassign("aaaa", "SRV1", tempListName);
        REQUIRE(descriptionRepository.exists("SRV1", tempListName));
        REQUIRE_FALSE(descriptionRepository.exists("aaaa", tempListName));
    }

    SECTION("reassign a parent: its children, also archived / deleted ones, follow the new id")
    {
        std::string child = listItemService.add(tempListName, "child", nullptr, nullptr, 0, "aaaa");
        listItemService.attach("bbbb", "aaaa", tempListName);
        std::string archivedChild = listItemService.archive("bbbb", tempListName);

        listItemService.reassign("aaaa", "SRV1", tempListName);
        REQUIRE(*listItemService.find(child, tempListName).getParentId() == "SRV1");
        REQUIRE(*listItemService.find(archivedChild, tempListNameArchive).getParentId() == "SRV1");

        listItemService.restore(archivedChild, tempListName);
        REQUIRE(*listItemService.find("bbbb", tempListName).getParentId() == "SRV1");
    }

    SECTION("reassign a child: it stays attached")
    {
        listItemService.attach("bbbb", "aaaa", tempListName);
        listItemService.reassign("bbbb", "kid1", tempListName);
        REQUIRE(*listItemService.find("kid1", tempListName).getParentId() == "aaaa");
    }
}
