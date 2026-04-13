#include "../../Actions/ListItemAction/AddItemAction.h"
#include "../../Actions/PriorityAction/PriorityAction.h"
#include "../../Actions/RemoveAction/RemoveAction.h"
#include "../../Actions/StatusAction/StatusAction.h"
#include "../../FileDataStorageRepositories/ListItemRepository.h"
#include "../../FileDataStorageRepositories/ListRepository.h"
#include "../../List/ListService.h"
#include "../../Serializers/ConfSerializer.h"
#include "../../Serializers/JsonSerializer.h"
#include "../Mock/MockAppInitialization.h"
#include "../Mock/MockAppInstallation.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Remove controller", "[CommandRouter][Remove]")
{
    IOService ioService("cli");
    ConfSerializer confService = ConfSerializer(ioService);
    JsonSerializer jsonService = JsonSerializer(ioService);
    std::unique_ptr<DataSerializerInterface> fileDataStorageServicePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> fileDataConfigStorageServicePtr =
        std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_CommandRouterTest");
    MockAppInstallation installation(ioService, jsonService, confService, init);

    installation.wipe();
    installation.make();
    EventBus bus = EventBus();
    Command command = Command("remove", { "aaaa" }, {}, "remove aaaa");
    ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
    ConfigService configService(ioService, init, configRepository, cacheRepository, command);

    PriorityService priorityService;
    StatusService statusService;
    ListItemRepository listItemRepository(
        configService, fileDataStorageServicePtr.get(), priorityService, statusService);
    ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
    ListRepository listRepository(configService, fileDataStorageServicePtr.get());
    ListService listService(ioService, configService, listRepository, bus);
    ListName listName = listService.createUsedListName();

    SECTION("remove with valid ID soft deletes item")
    {
        std::vector<ListItemEntity> itemsBefore = listItemService.get(listName);
        REQUIRE(itemsBefore.size() == 2);

        RemoveAction remove(ioService, command, listItemService);
        REQUIRE_NOTHROW(remove.remove(listName));

        std::vector<ListItemEntity> itemsAfter = listItemService.get(listName);
        REQUIRE(itemsAfter.size() == 1);
        REQUIRE(*itemsAfter[0].getId() == "bbbb");

        installation.wipe();
        installation.make();
    }

    SECTION("remove with empty arguments does not throw")
    {
        Command emptyCommand = Command("remove", {}, {}, "remove");
        RemoveAction remove(ioService, emptyCommand, listItemService);
        REQUIRE_NOTHROW(remove.remove(listName));

        // No items removed
        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 2);
    }

    SECTION("remove with force option permanently removes")
    {
        std::map<std::string, std::string> options = { { "force", "" } };
        Command forceCommand = Command("remove", { "aaaa" }, options, "remove -f aaaa");
        RemoveAction remove(ioService, forceCommand, listItemService);
        REQUIRE_NOTHROW(remove.remove(listName));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 1);

        // Item should NOT be in delete bin since it was force-removed
        ListName deleteListName = ListName::createVariant(listName, "delete");
        std::vector<ListItemEntity> deletedItems = listItemService.get(deleteListName);
        REQUIRE(deletedItems.size() == 0);

        installation.wipe();
        installation.make();
    }

    SECTION("archive with valid ID archives item")
    {
        Command archiveCommand = Command("archive", { "aaaa" }, {}, "archive aaaa");
        RemoveAction remove(ioService, archiveCommand, listItemService);
        REQUIRE_NOTHROW(remove.archive(listName));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 1);

        ListName archiveListName = ListName::createVariant(listName, "archive");
        std::vector<ListItemEntity> archivedItems = listItemService.get(archiveListName);
        REQUIRE(archivedItems.size() == 1);
        REQUIRE(*archivedItems[0].getId() == "aaaa");

        installation.wipe();
        installation.make();
    }

    SECTION("restore with valid ID restores item")
    {
        // First archive an item
        listItemService.archive("aaaa", listName);

        Command restoreCommand = Command("restore", { "aaaa" }, {}, "restore aaaa");
        RemoveAction remove(ioService, restoreCommand, listItemService);
        REQUIRE_NOTHROW(remove.restore(listName));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 2);

        installation.wipe();
        installation.make();
    }
}

TEST_CASE("Status controller", "[CommandRouter][Status]")
{
    IOService ioService("cli");
    ConfSerializer confService = ConfSerializer(ioService);
    JsonSerializer jsonService = JsonSerializer(ioService);
    std::unique_ptr<DataSerializerInterface> fileDataStorageServicePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> fileDataConfigStorageServicePtr =
        std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_CommandRouterTest2");
    MockAppInstallation installation(ioService, jsonService, confService, init);

    installation.wipe();
    installation.make();
    EventBus bus = EventBus();
    Command command = Command("start", { "aaaa" }, {}, "start aaaa");
    ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
    ConfigService configService(ioService, init, configRepository, cacheRepository, command);

    PriorityService priorityService;
    StatusService statusService;
    ListItemRepository listItemRepository(
        configService, fileDataStorageServicePtr.get(), priorityService, statusService);
    ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
    ListRepository listRepository(configService, fileDataStorageServicePtr.get());
    ListService listService(ioService, configService, listRepository, bus);
    ListName listName = listService.createUsedListName();

    SECTION("markAs changes item status")
    {
        StatusAction status(ioService, command, listItemService);
        REQUIRE_NOTHROW(status.markAs(listName, StatusService::STARTED));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.status()).getCommandName() == "started");

        installation.wipe();
        installation.make();
    }

    SECTION("markAs with empty arguments does not throw")
    {
        Command emptyCommand = Command("start", {}, {}, "start");
        StatusAction status(ioService, emptyCommand, listItemService);
        REQUIRE_NOTHROW(status.markAs(listName, StatusService::STARTED));
    }

    SECTION("set with valid status and ID changes status")
    {
        Command setCommand = Command("status", { "paused", "aaaa" }, {}, "status paused aaaa");
        StatusAction status(ioService, setCommand, listItemService);
        REQUIRE_NOTHROW(status.set(listName));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.status()).getCommandName() == "paused");

        installation.wipe();
        installation.make();
    }

    SECTION("set with invalid status does not throw")
    {
        Command badCommand = Command("status", { "invalidstatus", "aaaa" }, {}, "status invalidstatus aaaa");
        StatusAction status(ioService, badCommand, listItemService);
        REQUIRE_NOTHROW(status.set(listName));
    }

    SECTION("set with missing arguments does not throw")
    {
        Command noArgsCommand = Command("status", {}, {}, "status");
        StatusAction status(ioService, noArgsCommand, listItemService);
        REQUIRE_NOTHROW(status.set(listName));

        Command oneArgCommand = Command("status", { "paused" }, {}, "status paused");
        StatusAction status2(ioService, oneArgCommand, listItemService);
        REQUIRE_NOTHROW(status2.set(listName));
    }

    SECTION("reset with valid ID resets item")
    {
        // First change status
        listItemService.editStatus("aaaa", listName, new int(StatusService::STARTED));

        Command resetCommand = Command("reset", { "aaaa" }, {}, "reset aaaa");
        StatusAction status(ioService, resetCommand, listItemService);
        REQUIRE_NOTHROW(status.reset(listName));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.status()).getCommandName() == "to-do");

        installation.wipe();
        installation.make();
    }
}

TEST_CASE("Priority controller", "[CommandRouter][Priority]")
{
    IOService ioService("cli");
    ConfSerializer confService = ConfSerializer(ioService);
    JsonSerializer jsonService = JsonSerializer(ioService);
    std::unique_ptr<DataSerializerInterface> fileDataStorageServicePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> fileDataConfigStorageServicePtr =
        std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_CommandRouterTest3");
    MockAppInstallation installation(ioService, jsonService, confService, init);

    installation.wipe();
    installation.make();
    EventBus bus = EventBus();
    Command command = Command("increase", { "aaaa" }, {}, "increase aaaa");
    ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
    ConfigService configService(ioService, init, configRepository, cacheRepository, command);

    PriorityService priorityService;
    StatusService statusService;
    ListItemRepository listItemRepository(
        configService, fileDataStorageServicePtr.get(), priorityService, statusService);
    ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
    ListRepository listRepository(configService, fileDataStorageServicePtr.get());
    ListService listService(ioService, configService, listRepository, bus);
    ListName listName = listService.createUsedListName();

    SECTION("increase priority")
    {
        PriorityAction priority(ioService, command, listItemService);
        REQUIRE_NOTHROW(priority.increase(listName));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.priority()).getName() == "urgent");

        installation.wipe();
        installation.make();
    }

    SECTION("decrease priority")
    {
        Command decCommand = Command("decrease", { "aaaa" }, {}, "decrease aaaa");
        PriorityAction priority(ioService, decCommand, listItemService);
        REQUIRE_NOTHROW(priority.decrease(listName));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.priority()).getName() == "medium");

        installation.wipe();
        installation.make();
    }

    SECTION("increase with multiplier")
    {
        Command multiCommand = Command("increase", { "2", "aaaa" }, {}, "increase 2 aaaa");
        PriorityAction priority(ioService, multiCommand, listItemService);
        REQUIRE_NOTHROW(priority.increase(listName));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.priority()).getName() == "critical");

        installation.wipe();
        installation.make();
    }

    SECTION("set priority")
    {
        Command setCommand = Command("priority", { "low", "aaaa" }, {}, "priority low aaaa");
        PriorityAction priority(ioService, setCommand, listItemService);
        REQUIRE_NOTHROW(priority.set(listName));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.priority()).getName() == "low");

        installation.wipe();
        installation.make();
    }

    SECTION("set with empty arguments does not throw")
    {
        Command emptyCommand = Command("priority", {}, {}, "priority");
        PriorityAction priority(ioService, emptyCommand, listItemService);
        REQUIRE_NOTHROW(priority.set(listName));
    }

    SECTION("set with missing ID does not throw")
    {
        Command oneArgCommand = Command("priority", { "high" }, {}, "priority high");
        PriorityAction priority(ioService, oneArgCommand, listItemService);
        REQUIRE_NOTHROW(priority.set(listName));
    }
}

TEST_CASE("AddItemAction controller", "[CommandRouter][AddItemAction]")
{
    IOService ioService("cli");
    ConfSerializer confService = ConfSerializer(ioService);
    JsonSerializer jsonService = JsonSerializer(ioService);
    std::unique_ptr<DataSerializerInterface> fileDataStorageServicePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> fileDataConfigStorageServicePtr =
        std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_CommandRouterTest4");
    MockAppInstallation installation(ioService, jsonService, confService, init);

    installation.wipe();
    installation.make();
    EventBus bus = EventBus();
    ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());

    PriorityService priorityService;
    StatusService statusService;
    CommandRegistry commandList;
    CommandOption commandOption;
    CommandService commandService(commandList, commandOption);

    SECTION("add via AddItemAction")
    {
        Command addCommand = Command("add", { "new", "test", "item" }, {}, "add new test item");
        ConfigService configService(ioService, init, configRepository, cacheRepository, addCommand);
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        AddItemAction actions(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(actions.execute(addCommand, listName));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 3);

        // Find the new item
        bool found = false;
        for (auto& item : items) {
            if (*item.getValue() == "new test item") {
                found = true;
                break;
            }
        }
        REQUIRE(found);

        installation.wipe();
        installation.make();
    }

    SECTION("add with empty arguments does not throw")
    {
        Command emptyAddCommand = Command("add", {}, {}, "add");
        ConfigService configService(ioService, init, configRepository, cacheRepository, emptyAddCommand);
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        AddItemAction actions(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(actions.execute(emptyAddCommand, listName));

        // No item should have been added
        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 2);
    }

    SECTION("add with priority option")
    {
        std::map<std::string, std::string> options = { { "priority", "critical" } };
        Command addWithPrio = Command("add", { "urgent", "task" }, options, "add urgent task -p critical");
        ConfigService configService(ioService, init, configRepository, cacheRepository, addWithPrio);
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        AddItemAction actions(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(actions.execute(addWithPrio, listName));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 3);

        installation.wipe();
        installation.make();
    }

    SECTION("calculateValue joins arguments")
    {
        // This is tested indirectly via add - the value should be "word1 word2 word3"
        Command addCommand = Command("add", { "word1", "word2", "word3" }, {}, "add word1 word2 word3");
        ConfigService configService(ioService, init, configRepository, cacheRepository, addCommand);
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        AddItemAction actions(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(actions.execute(addCommand, listName));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        bool found = false;
        for (auto& item : items) {
            if (*item.getValue() == "word1 word2 word3") {
                found = true;
                break;
            }
        }
        REQUIRE(found);

        installation.wipe();
        installation.make();
    }
}
