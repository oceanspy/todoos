#include "../../Actions/ListItemAction/AppendItemAction.h"
#include "../../Actions/ListItemAction/DeadlineItemAction.h"
#include "../../Actions/ListItemAction/EditItemAction.h"
#include "../../Actions/ListItemAction/PrependItemAction.h"
#include "../../Actions/MoveAction/CopyItemAction.h"
#include "../../Command/CommandOption.h"
#include "../../Command/CommandRegistry.h"
#include "../../Command/CommandService.h"
#include "../../Config/ConfigService.h"
#include "../../Events/EventBus.h"
#include "../../FileDataStorageRepositories/ConfigRepository.h"
#include "../../FileDataStorageRepositories/ListItemRepository.h"
#include "../../FileDataStorageRepositories/ListRepository.h"
#include "../../List/ListItemService.h"
#include "../../List/ListItems/PriorityService.h"
#include "../../List/ListItems/StatusService.h"
#include "../../List/ListService.h"
#include "../../Serializers/ConfSerializer.h"
#include "../../Serializers/JsonSerializer.h"
#include "../Mock/MockAppInitialization.h"
#include "../Mock/MockAppInstallation.h"
#include <catch2/catch_test_macros.hpp>

// ---------------------------------------------------------------------------
// AppendItemAction
// ---------------------------------------------------------------------------

TEST_CASE("AppendItemAction", "[Actions][AppendItem]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);

    MockAppInitialization init(ioService, "_todoos_AppendItemActionTest");
    MockAppInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    CommandRegistry commandList;
    CommandOption commandOption;
    CommandService commandService(commandList, commandOption);
    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());
    PriorityService priorityService;
    StatusService statusService;

    SECTION("append adds text to end of item value")
    {
        Command command("append", { "aaaa", "appended", "text" }, {}, "append aaaa appended text");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        AppendItemAction action(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(action.execute(command, listName));

        ListItemEntity item = listItemService.find("aaaa", listName);
        std::string value = *item.getValue();
        REQUIRE(value.find("appended text") != std::string::npos);
        REQUIRE(value.rfind("appended text") == value.size() - std::string("appended text").size());

        installation.wipe();
        installation.make();
    }

    SECTION("append with no arguments does not throw")
    {
        Command command("append", {}, {}, "append");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        AppendItemAction action(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(action.execute(command, listName));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 2);
    }

    SECTION("append with only ID and no text does not throw")
    {
        Command command("append", { "aaaa" }, {}, "append aaaa");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        AppendItemAction action(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(action.execute(command, listName));
    }

    SECTION("append with invalid ID does not throw")
    {
        Command command("append", { "!!!", "some", "text" }, {}, "append !!! some text");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        AppendItemAction action(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(action.execute(command, listName));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 2);
    }

    SECTION("append with non-existent ID does not throw")
    {
        Command command("append", { "zzzz", "some", "text" }, {}, "append zzzz some text");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        AppendItemAction action(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(action.execute(command, listName));
    }

    SECTION("append with priority option changes item priority")
    {
        std::map<std::string, std::string> options = { { "priority", "low" } };
        Command command("append", { "aaaa", "extra" }, options, "append aaaa extra -p low");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        AppendItemAction action(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(action.execute(command, listName));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.priority()).getName() == "low");

        installation.wipe();
        installation.make();
    }

    SECTION("append with status option changes item status")
    {
        std::map<std::string, std::string> options = { { "status", "paused" } };
        Command command("append", { "aaaa", "extra" }, options, "append aaaa extra -s paused");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        AppendItemAction action(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(action.execute(command, listName));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.status()).getCommandName() == "paused");

        installation.wipe();
        installation.make();
    }

    SECTION("append with invalid priority option does not throw and leaves priority unchanged")
    {
        std::map<std::string, std::string> options = { { "priority", "notapriority" } };
        Command command("append", { "aaaa", "extra" }, options, "append aaaa extra -p notapriority");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        AppendItemAction action(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(action.execute(command, listName));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.priority()).getName() == "high");

        installation.wipe();
        installation.make();
    }
}

// ---------------------------------------------------------------------------
// PrependItemAction
// ---------------------------------------------------------------------------

TEST_CASE("PrependItemAction", "[Actions][PrependItem]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);

    MockAppInitialization init(ioService, "_todoos_PrependItemActionTest");
    MockAppInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    CommandRegistry commandList;
    CommandOption commandOption;
    CommandService commandService(commandList, commandOption);
    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());
    PriorityService priorityService;
    StatusService statusService;

    SECTION("prepend adds text to beginning of item value")
    {
        Command command("prepend", { "aaaa", "prefix", "text" }, {}, "prepend aaaa prefix text");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        PrependItemAction action(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(action.execute(command, listName));

        ListItemEntity item = listItemService.find("aaaa", listName);
        std::string value = *item.getValue();
        REQUIRE(value.find("prefix text") == 0);

        installation.wipe();
        installation.make();
    }

    SECTION("prepend with no arguments does not throw")
    {
        Command command("prepend", {}, {}, "prepend");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        PrependItemAction action(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(action.execute(command, listName));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 2);
    }

    SECTION("prepend with only ID does not throw")
    {
        Command command("prepend", { "aaaa" }, {}, "prepend aaaa");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        PrependItemAction action(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(action.execute(command, listName));
    }

    SECTION("prepend with invalid ID does not throw")
    {
        Command command("prepend", { "@#$%", "prefix" }, {}, "prepend @#$% prefix");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        PrependItemAction action(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(action.execute(command, listName));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 2);
    }

    SECTION("prepend with non-existent ID does not throw")
    {
        Command command("prepend", { "zzzz", "prefix" }, {}, "prepend zzzz prefix");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        PrependItemAction action(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(action.execute(command, listName));
    }

    SECTION("prepend with priority option changes item priority")
    {
        std::map<std::string, std::string> options = { { "priority", "low" } };
        Command command("prepend", { "aaaa", "note" }, options, "prepend aaaa note -p low");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        PrependItemAction action(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(action.execute(command, listName));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.priority()).getName() == "low");

        installation.wipe();
        installation.make();
    }

    SECTION("prepend result differs from append result")
    {
        Command appendCmd("append", { "aaaa", "suffix" }, {}, "append aaaa suffix");
        Command prependCmd("prepend", { "bbbb", "prefix" }, {}, "prepend bbbb prefix");
        ConfigService configService(ioService, init, configRepository, cacheRepository, appendCmd);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        AppendItemAction appendAction(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(appendAction.execute(appendCmd, listName));

        PrependItemAction prependAction(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(prependAction.execute(prependCmd, listName));

        ListItemEntity appendedItem = listItemService.find("aaaa", listName);
        std::string appendedValue = *appendedItem.getValue();
        REQUIRE(appendedValue.rfind("suffix") == appendedValue.size() - std::string("suffix").size());

        ListItemEntity prependedItem = listItemService.find("bbbb", listName);
        std::string prependedValue = *prependedItem.getValue();
        REQUIRE(prependedValue.find("prefix") == 0);

        installation.wipe();
        installation.make();
    }
}

// ---------------------------------------------------------------------------
// EditItemAction
// ---------------------------------------------------------------------------

TEST_CASE("EditItemAction", "[Actions][EditItem]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);

    MockAppInitialization init(ioService, "_todoos_EditItemActionTest");
    MockAppInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    CommandRegistry commandList;
    CommandOption commandOption;
    CommandService commandService(commandList, commandOption);
    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());
    PriorityService priorityService;
    StatusService statusService;

    SECTION("edit replaces item value")
    {
        Command command("edit", { "aaaa", "new", "value" }, {}, "edit aaaa new value");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        EditItemAction action(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(action.execute(command, listName));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*item.getValue() == "new value");

        installation.wipe();
        installation.make();
    }

    SECTION("edit with no arguments does not throw")
    {
        Command command("edit", {}, {}, "edit");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        EditItemAction action(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(action.execute(command, listName));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 2);
    }

    SECTION("edit with invalid ID does not throw")
    {
        Command command("edit", { "!!!", "new", "value" }, {}, "edit !!! new value");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        EditItemAction action(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(action.execute(command, listName));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 2);
    }

    SECTION("edit with priority option changes priority")
    {
        std::map<std::string, std::string> options = { { "priority", "critical" } };
        Command command("edit", { "aaaa" }, options, "edit aaaa -p critical");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        EditItemAction action(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(action.execute(command, listName));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.priority()).getName() == "critical");

        installation.wipe();
        installation.make();
    }

    SECTION("edit with status option changes status")
    {
        std::map<std::string, std::string> options = { { "status", "completed" } };
        Command command("edit", { "aaaa" }, options, "edit aaaa -s completed");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        EditItemAction action(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(action.execute(command, listName));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.status()).getCommandName() == "completed");

        installation.wipe();
        installation.make();
    }

    SECTION("edit with invalid priority option does not throw and leaves priority unchanged")
    {
        std::map<std::string, std::string> options = { { "priority", "bogus" } };
        Command command("edit", { "aaaa", "new", "text" }, options, "edit aaaa new text -p bogus");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        EditItemAction action(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(action.execute(command, listName));

        installation.wipe();
        installation.make();
    }

    SECTION("edit with invalid deadline option does not throw")
    {
        std::map<std::string, std::string> options = { { "deadline", "notadate" } };
        Command command("edit", { "aaaa", "text" }, options, "edit aaaa text --deadline notadate");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        EditItemAction action(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(action.execute(command, listName));
    }

    SECTION("edit with non-existent ID does not throw")
    {
        Command command("edit", { "zzzz", "new", "value" }, {}, "edit zzzz new value");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        EditItemAction action(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(action.execute(command, listName));
    }

    SECTION("edit does not alter item count")
    {
        Command command("edit", { "aaaa", "updated", "value" }, {}, "edit aaaa updated value");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        EditItemAction action(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(action.execute(command, listName));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 2);

        installation.wipe();
        installation.make();
    }
}

// ---------------------------------------------------------------------------
// DeadlineItemAction
// ---------------------------------------------------------------------------

TEST_CASE("DeadlineItemAction", "[Actions][DeadlineItem]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);

    MockAppInitialization init(ioService, "_todoos_DeadlineItemActionTest");
    MockAppInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    CommandRegistry commandList;
    CommandOption commandOption;
    CommandService commandService(commandList, commandOption);
    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());
    PriorityService priorityService;
    StatusService statusService;

    SECTION("deadline sets due date on item")
    {
        Command command("deadline", { "2031.01.15", "aaaa" }, {}, "deadline 2031.01.15 aaaa");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        DeadlineItemAction action(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(action.execute(command, listName));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*item.getDueAt() > 0);

        installation.wipe();
        installation.make();
    }

    SECTION("deadline with no arguments does not throw")
    {
        Command command("deadline", {}, {}, "deadline");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        DeadlineItemAction action(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(action.execute(command, listName));
    }

    SECTION("deadline with only date and no ID does not throw")
    {
        Command command("deadline", { "2031.01.15" }, {}, "deadline 2031.01.15");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        DeadlineItemAction action(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(action.execute(command, listName));
    }

    SECTION("deadline with invalid ID does not throw")
    {
        Command command("deadline", { "2031.01.15", "!!!" }, {}, "deadline 2031.01.15 !!!");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        DeadlineItemAction action(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(action.execute(command, listName));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*item.getDueAt() == 0);
    }

    SECTION("deadline with multiple IDs sets all deadlines")
    {
        Command command("deadline", { "2031.06.01", "aaaa", "bbbb" }, {}, "deadline 2031.06.01 aaaa bbbb");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        DeadlineItemAction action(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(action.execute(command, listName));

        ListItemEntity itemA = listItemService.find("aaaa", listName);
        ListItemEntity itemB = listItemService.find("bbbb", listName);
        REQUIRE(*itemA.getDueAt() > 0);
        REQUIRE(*itemB.getDueAt() > 0);

        installation.wipe();
        installation.make();
    }

    SECTION("deadline with duplicate IDs deduplicates and sets deadline once")
    {
        Command command("deadline", { "2031.06.01", "aaaa", "aaaa" }, {}, "deadline 2031.06.01 aaaa aaaa");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        DeadlineItemAction action(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(action.execute(command, listName));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*item.getDueAt() > 0);

        installation.wipe();
        installation.make();
    }

    SECTION("deadline with non-existent ID does not throw")
    {
        Command command("deadline", { "2031.06.01", "zzzz" }, {}, "deadline 2031.06.01 zzzz");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        DeadlineItemAction action(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(action.execute(command, listName));
    }
}

// ---------------------------------------------------------------------------
// CopyItemAction
// ---------------------------------------------------------------------------

TEST_CASE("CopyItemAction", "[Actions][CopyItem]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);

    MockAppInitialization init(ioService, "_todoos_CopyItemActionTest");
    MockAppInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());
    PriorityService priorityService;
    StatusService statusService;

    SECTION("copy moves item to target list while keeping it in source")
    {
        std::map<std::string, std::string> forceOpt = { { "force", "" } };
        Command command("copy", { "tempList2Name", "aaaa" }, forceOpt, "copy --force tempList2Name aaaa");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();
        ListName targetListName = listService.createListName("tempList2Name");

        CopyItemAction action(ioService, listService, listItemService);
        REQUIRE_NOTHROW(action.execute(command, listName));

        std::vector<ListItemEntity> sourceItems = listItemService.get(listName);
        REQUIRE(sourceItems.size() == 2);

        std::vector<ListItemEntity> targetItems = listItemService.get(targetListName);
        bool found = false;
        for (const auto& item : targetItems) {
            if (*item.getId() == "aaaa") {
                found = true;
                break;
            }
        }
        REQUIRE(found);

        installation.wipe();
        installation.make();
    }

    SECTION("copy with no arguments does not throw")
    {
        Command command("copy", {}, {}, "copy");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        CopyItemAction action(ioService, listService, listItemService);
        REQUIRE_NOTHROW(action.execute(command, listName));
    }

    SECTION("copy with only list name and no ID does not throw")
    {
        Command command("copy", { "tempList2Name" }, {}, "copy tempList2Name");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        CopyItemAction action(ioService, listService, listItemService);
        REQUIRE_NOTHROW(action.execute(command, listName));
    }

    SECTION("copy with force option overwrites existing item")
    {
        std::map<std::string, std::string> options = { { "force", "" } };
        Command command("copy", { "tempList2Name", "aaaa" }, options, "copy --force tempList2Name aaaa");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        CopyItemAction action(ioService, listService, listItemService);
        REQUIRE_NOTHROW(action.execute(command, listName));

        ListName targetListName = listService.createListName("tempList2Name");
        bool found = false;
        for (const auto& item : listItemService.get(targetListName)) {
            if (*item.getId() == "aaaa") {
                found = true;
                break;
            }
        }
        REQUIRE(found);

        installation.wipe();
        installation.make();
    }

    SECTION("copy with non-existent source ID does not throw")
    {
        Command command("copy", { "tempList2Name", "zzzz" }, {}, "copy tempList2Name zzzz");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        CopyItemAction action(ioService, listService, listItemService);
        REQUIRE_NOTHROW(action.execute(command, listName));
    }
}
