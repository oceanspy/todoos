#include "../../Command/CommandOption.h"
#include "../../Command/CommandRegistry.h"
#include "../../Command/CommandService.h"
#include "../../Events/EventBus.h"
#include "../../FileDataStorageRepositories/ConfigRepository.h"
#include "../../FileDataStorageRepositories/ListItemRepository.h"
#include "../../FileDataStorageRepositories/ListRepository.h"
#include "../../Serializers/ConfSerializer.h"
#include "../../Serializers/JsonSerializer.h"
#include "../../Themes/ThemeService.h"
#include "../../UseCase/AppendItemUseCase.h"
#include "../../UseCase/CopyItemUseCase.h"
#include "../../UseCase/DeadlineItemUseCase.h"
#include "../../UseCase/DuplicateItemUseCase.h"
#include "../../UseCase/EditItemUseCase.h"
#include "../../UseCase/PrependItemUseCase.h"
#include "../Mock/MockAppInitialization.h"
#include "../Mock/MockAppInstallation.h"
#include <catch2/catch_test_macros.hpp>

// ---------------------------------------------------------------------------
// AppendItemUseCase
// ---------------------------------------------------------------------------

TEST_CASE("AppendItemUseCase", "[UseCase][AppendItem]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_UCAppendItem");
    MockAppInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    CommandRegistry commandList;
    CommandOption commandOption;
    CommandService commandService(commandList, commandOption);
    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());

    SECTION("appended text appears at end of item value")
    {
        Command command("append", { "aaaa", "extra", "suffix" }, {}, "append aaaa extra suffix");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);
        ListName listName = listService.createUsedListName();

        REQUIRE_NOTHROW(
            AppendItemUseCase(ioService, commandService, listItemService, listService, configService, themeService)
                .execute(command));

        ListItemEntity item = listItemService.find("aaaa", listName);
        std::string value = *item.getValue();
        REQUIRE(value.rfind("extra suffix") == value.size() - std::string("extra suffix").size());

        installation.wipe();
        installation.make();
    }

    SECTION("append does not change item count")
    {
        Command command("append", { "aaaa", "more", "text" }, {}, "append aaaa more text");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);
        ListName listName = listService.createUsedListName();

        REQUIRE_NOTHROW(
            AppendItemUseCase(ioService, commandService, listItemService, listService, configService, themeService)
                .execute(command));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 2);

        installation.wipe();
        installation.make();
    }

    SECTION("append with no arguments does not throw")
    {
        Command command("append", {}, {}, "append");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);

        REQUIRE_NOTHROW(
            AppendItemUseCase(ioService, commandService, listItemService, listService, configService, themeService)
                .execute(command));
    }

    SECTION("append with invalid ID does not throw")
    {
        Command command("append", { "!!!", "text" }, {}, "append !!! text");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);

        REQUIRE_NOTHROW(
            AppendItemUseCase(ioService, commandService, listItemService, listService, configService, themeService)
                .execute(command));
    }
}

// ---------------------------------------------------------------------------
// PrependItemUseCase
// ---------------------------------------------------------------------------

TEST_CASE("PrependItemUseCase", "[UseCase][PrependItem]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_UCPrependItem");
    MockAppInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    CommandRegistry commandList;
    CommandOption commandOption;
    CommandService commandService(commandList, commandOption);
    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());

    SECTION("prepended text appears at beginning of item value")
    {
        Command command("prepend", { "aaaa", "prefix", "words" }, {}, "prepend aaaa prefix words");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);
        ListName listName = listService.createUsedListName();

        REQUIRE_NOTHROW(
            PrependItemUseCase(ioService, commandService, listItemService, listService, configService, themeService)
                .execute(command));

        ListItemEntity item = listItemService.find("aaaa", listName);
        std::string value = *item.getValue();
        REQUIRE(value.find("prefix words") == 0);

        installation.wipe();
        installation.make();
    }

    SECTION("prepend does not change item count")
    {
        Command command("prepend", { "aaaa", "note" }, {}, "prepend aaaa note");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);
        ListName listName = listService.createUsedListName();

        REQUIRE_NOTHROW(
            PrependItemUseCase(ioService, commandService, listItemService, listService, configService, themeService)
                .execute(command));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 2);

        installation.wipe();
        installation.make();
    }

    SECTION("prepend with no arguments does not throw")
    {
        Command command("prepend", {}, {}, "prepend");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);

        REQUIRE_NOTHROW(
            PrependItemUseCase(ioService, commandService, listItemService, listService, configService, themeService)
                .execute(command));
    }

    SECTION("prepend with invalid ID does not throw")
    {
        Command command("prepend", { "@#$", "text" }, {}, "prepend @#$ text");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);

        REQUIRE_NOTHROW(
            PrependItemUseCase(ioService, commandService, listItemService, listService, configService, themeService)
                .execute(command));
    }
}

// ---------------------------------------------------------------------------
// EditItemUseCase
// ---------------------------------------------------------------------------

TEST_CASE("EditItemUseCase", "[UseCase][EditItem]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_UCEditItem");
    MockAppInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    CommandRegistry commandList;
    CommandOption commandOption;
    CommandService commandService(commandList, commandOption);
    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());

    SECTION("edit replaces item value")
    {
        Command command("edit", { "aaaa", "replaced", "value" }, {}, "edit aaaa replaced value");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);
        ListName listName = listService.createUsedListName();

        REQUIRE_NOTHROW(
            EditItemUseCase(ioService, commandService, listItemService, listService, configService, themeService)
                .execute(command));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*item.getValue() == "replaced value");

        installation.wipe();
        installation.make();
    }

    SECTION("edit does not change item count")
    {
        Command command("edit", { "aaaa", "new", "text" }, {}, "edit aaaa new text");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);
        ListName listName = listService.createUsedListName();

        REQUIRE_NOTHROW(
            EditItemUseCase(ioService, commandService, listItemService, listService, configService, themeService)
                .execute(command));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 2);

        installation.wipe();
        installation.make();
    }

    SECTION("edit with no arguments does not throw")
    {
        Command command("edit", {}, {}, "edit");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);

        REQUIRE_NOTHROW(
            EditItemUseCase(ioService, commandService, listItemService, listService, configService, themeService)
                .execute(command));
    }

    SECTION("edit with priority option changes priority")
    {
        std::map<std::string, std::string> options = { { "priority", "low" } };
        Command command("edit", { "aaaa" }, options, "edit aaaa -p low");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);
        ListName listName = listService.createUsedListName();

        REQUIRE_NOTHROW(
            EditItemUseCase(ioService, commandService, listItemService, listService, configService, themeService)
                .execute(command));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.priority()).getName() == "low");

        installation.wipe();
        installation.make();
    }

    SECTION("edit with status option changes status")
    {
        std::map<std::string, std::string> options = { { "status", "reviewing" } };
        Command command("edit", { "aaaa" }, options, "edit aaaa -s reviewing");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);
        ListName listName = listService.createUsedListName();

        REQUIRE_NOTHROW(
            EditItemUseCase(ioService, commandService, listItemService, listService, configService, themeService)
                .execute(command));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.status()).getCommandName() == "reviewing");

        installation.wipe();
        installation.make();
    }

    SECTION("edit with invalid ID does not throw")
    {
        Command command("edit", { "!!!", "new", "value" }, {}, "edit !!! new value");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);

        REQUIRE_NOTHROW(
            EditItemUseCase(ioService, commandService, listItemService, listService, configService, themeService)
                .execute(command));
    }

    SECTION("edit with invalid deadline option does not throw")
    {
        std::map<std::string, std::string> options = { { "deadline", "notadate" } };
        Command command("edit", { "aaaa", "text" }, options, "edit aaaa text --deadline notadate");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);

        REQUIRE_NOTHROW(
            EditItemUseCase(ioService, commandService, listItemService, listService, configService, themeService)
                .execute(command));
    }
}

// ---------------------------------------------------------------------------
// DeadlineItemUseCase
// ---------------------------------------------------------------------------

TEST_CASE("DeadlineItemUseCase", "[UseCase][DeadlineItem]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_UCDeadlineItem");
    MockAppInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    CommandRegistry commandList;
    CommandOption commandOption;
    CommandService commandService(commandList, commandOption);
    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());

    SECTION("deadline sets due date on item")
    {
        Command command("deadline", { "2031.03.20", "aaaa" }, {}, "deadline 2031.03.20 aaaa");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);
        ListName listName = listService.createUsedListName();

        REQUIRE_NOTHROW(
            DeadlineItemUseCase(ioService, commandService, listItemService, listService, configService, themeService)
                .execute(command));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*item.getDueAt() > 0);

        installation.wipe();
        installation.make();
    }

    SECTION("deadline with no arguments does not throw")
    {
        Command command("deadline", {}, {}, "deadline");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);

        REQUIRE_NOTHROW(
            DeadlineItemUseCase(ioService, commandService, listItemService, listService, configService, themeService)
                .execute(command));
    }

    SECTION("deadline does not change item count")
    {
        Command command("deadline", { "2031.03.20", "aaaa" }, {}, "deadline 2031.03.20 aaaa");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);
        ListName listName = listService.createUsedListName();

        REQUIRE_NOTHROW(
            DeadlineItemUseCase(ioService, commandService, listItemService, listService, configService, themeService)
                .execute(command));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 2);

        installation.wipe();
        installation.make();
    }

    SECTION("deadline with invalid ID does not throw")
    {
        Command command("deadline", { "2031.03.20", "!!!" }, {}, "deadline 2031.03.20 !!!");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);

        REQUIRE_NOTHROW(
            DeadlineItemUseCase(ioService, commandService, listItemService, listService, configService, themeService)
                .execute(command));
    }
}

// ---------------------------------------------------------------------------
// CopyItemUseCase
// ---------------------------------------------------------------------------

TEST_CASE("CopyItemUseCase", "[UseCase][CopyItem]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_UCCopyItem");
    MockAppInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());

    SECTION("copy places item in target list")
    {
        std::map<std::string, std::string> forceOpt = { { "force", "" } };
        Command command("copy", { "tempList2Name", "aaaa" }, forceOpt, "copy --force tempList2Name aaaa");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);
        ListName listName = listService.createUsedListName();
        ListName targetListName = listService.createListName("tempList2Name");

        REQUIRE_NOTHROW(
            CopyItemUseCase(ioService, listService, listItemService, configService, themeService).execute(command));

        std::vector<ListItemEntity> sourceItems = listItemService.get(listName);
        REQUIRE(sourceItems.size() == 2);

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

    SECTION("copy with no arguments does not throw")
    {
        Command command("copy", {}, {}, "copy");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);

        REQUIRE_NOTHROW(
            CopyItemUseCase(ioService, listService, listItemService, configService, themeService).execute(command));
    }

    SECTION("copy with non-existent ID does not throw")
    {
        Command command("copy", { "tempList2Name", "zzzz" }, {}, "copy tempList2Name zzzz");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);

        REQUIRE_NOTHROW(
            CopyItemUseCase(ioService, listService, listItemService, configService, themeService).execute(command));
    }
}

// ---------------------------------------------------------------------------
// DuplicateItemUseCase
// ---------------------------------------------------------------------------

TEST_CASE("DuplicateItemUseCase", "[UseCase][DuplicateItem]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_UCDuplicateItem");
    MockAppInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());

    SECTION("duplicate increases item count by one")
    {
        Command command("duplicate", { "aaaa" }, {}, "duplicate aaaa");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);
        ListName listName = listService.createUsedListName();

        REQUIRE_NOTHROW(DuplicateItemUseCase(ioService, listService, listItemService, configService, themeService)
                            .execute(command));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 3);

        installation.wipe();
        installation.make();
    }

    SECTION("duplicated item has same value as original")
    {
        Command command("duplicate", { "aaaa" }, {}, "duplicate aaaa");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);
        ListName listName = listService.createUsedListName();

        REQUIRE_NOTHROW(DuplicateItemUseCase(ioService, listService, listItemService, configService, themeService)
                            .execute(command));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        int matchCount = 0;
        for (const auto& item : items) {
            if (*item.getValue() == "test 1") {
                matchCount++;
            }
        }
        REQUIRE(matchCount == 2);

        installation.wipe();
        installation.make();
    }

    SECTION("duplicated item has a different ID than original")
    {
        Command command("duplicate", { "aaaa" }, {}, "duplicate aaaa");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);
        ListName listName = listService.createUsedListName();

        REQUIRE_NOTHROW(DuplicateItemUseCase(ioService, listService, listItemService, configService, themeService)
                            .execute(command));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        std::vector<std::string> ids;
        for (const auto& item : items) {
            ids.push_back(*item.getId());
        }
        std::sort(ids.begin(), ids.end());
        auto duplicateIt = std::adjacent_find(ids.begin(), ids.end());
        REQUIRE(duplicateIt == ids.end());

        installation.wipe();
        installation.make();
    }

    SECTION("duplicate with no arguments does not throw")
    {
        Command command("duplicate", {}, {}, "duplicate");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);
        ListName listName = listService.createUsedListName();

        REQUIRE_NOTHROW(DuplicateItemUseCase(ioService, listService, listItemService, configService, themeService)
                            .execute(command));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 2);
    }

    SECTION("duplicate with non-existent ID does not throw")
    {
        Command command("duplicate", { "zzzz" }, {}, "duplicate zzzz");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);

        REQUIRE_NOTHROW(DuplicateItemUseCase(ioService, listService, listItemService, configService, themeService)
                            .execute(command));
    }
}

// ---------------------------------------------------------------------------
// Cross-feature integration tests
// ---------------------------------------------------------------------------

TEST_CASE("Integration: add then edit then append sequence", "[Integration][Sequence]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_IntegrationSequence");
    MockAppInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    CommandRegistry commandList;
    CommandOption commandOption;
    CommandService commandService(commandList, commandOption);
    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());

    SECTION("add then edit then append yields correct final value")
    {
        Command addCmd("add", { "initial", "value" }, {}, "add initial value");
        ConfigService configService(ioService, init, configRepository, cacheRepository, addCmd);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);
        ListName listName = listService.createUsedListName();

        std::string newId = listItemService.add(listName, "initial value");
        REQUIRE(!newId.empty());

        listItemService.edit(newId, listName, "edited value");
        ListItemEntity editedItem = listItemService.find(newId, listName);
        REQUIRE(*editedItem.getValue() == "edited value");

        listItemService.append(newId, listName, "appended");
        ListItemEntity appendedItem = listItemService.find(newId, listName);
        std::string finalValue = *appendedItem.getValue();
        REQUIRE(finalValue.find("appended") != std::string::npos);
        REQUIRE(finalValue.rfind("appended") == finalValue.size() - std::string("appended").size());

        installation.wipe();
        installation.make();
    }
}

TEST_CASE("Integration: archive then restore sequence", "[Integration][ArchiveRestore]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_IntegrationArchive");
    MockAppInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    Command command("archive", { "aaaa" }, {}, "archive aaaa");
    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());
    ConfigService configService(ioService, init, configRepository, cacheRepository, command);
    PriorityService priorityService;
    StatusService statusService;
    ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
    ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
    ListRepository listRepository(configService, storagePtr.get());
    ListService listService(ioService, configService, listRepository, bus);
    ListName listName = listService.createUsedListName();

    SECTION("archive then restore returns item to main list")
    {
        listItemService.archive("aaaa", listName);
        std::vector<ListItemEntity> afterArchive = listItemService.get(listName);
        REQUIRE(afterArchive.size() == 1);

        ListName archiveName = ListName::createVariant(listName, "archive");
        std::vector<ListItemEntity> inArchive = listItemService.get(archiveName);
        REQUIRE(inArchive.size() == 1);

        listItemService.restore("aaaa", listName);
        std::vector<ListItemEntity> afterRestore = listItemService.get(listName);
        REQUIRE(afterRestore.size() == 2);

        std::vector<ListItemEntity> archiveAfterRestore = listItemService.get(archiveName);
        REQUIRE(archiveAfterRestore.size() == 0);

        installation.wipe();
        installation.make();
    }

    SECTION("archive then restore then archive again works")
    {
        listItemService.archive("aaaa", listName);
        listItemService.restore("aaaa", listName);
        listItemService.archive("aaaa", listName);

        std::vector<ListItemEntity> mainItems = listItemService.get(listName);
        REQUIRE(mainItems.size() == 1);

        ListName archiveName = ListName::createVariant(listName, "archive");
        std::vector<ListItemEntity> archiveItems = listItemService.get(archiveName);
        REQUIRE(archiveItems.size() == 1);
        REQUIRE(*archiveItems[0].getId() == "aaaa");

        installation.wipe();
        installation.make();
    }
}

TEST_CASE("Integration: duplicate preserves value and priority", "[Integration][Duplicate]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_IntegrationDuplicate");
    MockAppInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    Command command("duplicate", { "aaaa" }, {}, "duplicate aaaa");
    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());
    ConfigService configService(ioService, init, configRepository, cacheRepository, command);
    PriorityService priorityService;
    StatusService statusService;
    ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
    ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
    ListRepository listRepository(configService, storagePtr.get());
    ListService listService(ioService, configService, listRepository, bus);
    ListName listName = listService.createUsedListName();

    SECTION("duplicate copies value and priority from original")
    {
        listItemService.duplicate("aaaa", listName);
        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 3);

        int highPriorityCount = 0;
        int matchingValueCount = 0;
        for (const auto& item : items) {
            if (*item.getValue() == "test 1") {
                matchingValueCount++;
            }
            if (*(*item.priority()).getName() == "high") {
                highPriorityCount++;
            }
        }
        REQUIRE(matchingValueCount == 2);
        REQUIRE(highPriorityCount >= 1);

        installation.wipe();
        installation.make();
    }
}

TEST_CASE("Integration: boundary cases on empty-ish operations", "[Integration][Boundary]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_IntegrationBoundary");
    MockAppInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    Command command("remove", { "aaaa" }, {}, "remove aaaa");
    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());
    ConfigService configService(ioService, init, configRepository, cacheRepository, command);
    PriorityService priorityService;
    StatusService statusService;
    ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
    ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
    ListRepository listRepository(configService, storagePtr.get());
    ListService listService(ioService, configService, listRepository, bus);
    ListName listName = listService.createUsedListName();

    SECTION("remove all items then try to remove again throws")
    {
        listItemService.softDelete("aaaa", listName);
        listItemService.softDelete("bbbb", listName);

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 0);

        REQUIRE_THROWS(listItemService.softDelete("aaaa", listName));
    }

    SECTION("prepend on newly added item produces correct result")
    {
        std::string newId = listItemService.add(listName, "base");
        REQUIRE_NOTHROW(listItemService.prepend(newId, listName, "prefix"));
        ListItemEntity item = listItemService.find(newId, listName);
        REQUIRE(*item.getValue() == "prefix base");

        installation.wipe();
        installation.make();
    }

    SECTION("append on newly added item produces correct result")
    {
        std::string newId = listItemService.add(listName, "base");
        REQUIRE_NOTHROW(listItemService.append(newId, listName, "suffix"));
        ListItemEntity item = listItemService.find(newId, listName);
        REQUIRE(*item.getValue() == "base suffix");

        installation.wipe();
        installation.make();
    }

    SECTION("increase priority at maximum throws")
    {
        listItemService.setPriority("aaaa", listName, new std::string("critical"));
        REQUIRE_THROWS(listItemService.increasePriority("aaaa", listName));
    }

    SECTION("decrease priority at minimum throws")
    {
        listItemService.setPriority("aaaa", listName, new std::string("low"));
        REQUIRE_THROWS(listItemService.decreasePriority("aaaa", listName));
    }
}
