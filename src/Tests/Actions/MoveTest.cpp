#include "../../Actions/Move/Move.h"
#include "../../Command/CommandList.h"
#include "../../Command/CommandOption.h"
#include "../../Command/CommandService.h"
#include "../../Config/ConfigService.h"
#include "../../Events/EventBus.h"
#include "../../FileDataStorage/ConfService.h"
#include "../../FileDataStorage/JSONService.h"
#include "../../FileDataStorageRepositories/ConfigRepository.h"
#include "../../FileDataStorageRepositories/ListItemRepository.h"
#include "../../FileDataStorageRepositories/ListRepository.h"
#include "../../List/ListItemService.h"
#include "../../List/ListItems/PriorityService.h"
#include "../../List/ListItems/StatusService.h"
#include "../../List/ListService.h"
#include "../Mock/MockInit.h"
#include "../Mock/MockInstallation.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Move action", "[Move]")
{
    IOService ioService("cli");
    ConfService confService(ioService);
    JSONService jsonService(ioService);
    std::unique_ptr<FileDataServiceInterface> storagePtr = std::make_unique<JSONService>(ioService);
    std::unique_ptr<FileDataServiceInterface> configStoragePtr = std::make_unique<ConfService>(ioService);

    MockInit init(ioService, "_todoos_MoveActionTest");
    MockInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    CommandList commandList;
    CommandOption commandOption;
    CommandService commandService(commandList, commandOption);

    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());

    PriorityService priorityService;
    StatusService statusService;

    // ---- make() — move-to ------------------------------------------------------

    SECTION("make — move-to moves item to target list")
    {
        Command command("move-to", { "tempList2Name", "aaaa" }, {}, "move-to tempList2Name aaaa");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<FileDataServiceInterface> listStoragePtr = std::make_unique<JSONService>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        Move move(ioService, command, commandService, listService, listItemService);
        REQUIRE_NOTHROW(move.make(listName));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 1);
        REQUIRE(*items[0].getId() == "bbbb");

        installation.wipe();
        installation.make();
    }

    SECTION("make — move-to with too few arguments does not throw")
    {
        Command command("move-to", { "tempList2Name" }, {}, "move-to tempList2Name");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<FileDataServiceInterface> listStoragePtr = std::make_unique<JSONService>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        Move move(ioService, command, commandService, listService, listItemService);
        REQUIRE_NOTHROW(move.make(listName));
    }

    SECTION("make — empty arguments does not throw")
    {
        Command command("move-to", {}, {}, "move-to");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<FileDataServiceInterface> listStoragePtr = std::make_unique<JSONService>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        Move move(ioService, command, commandService, listService, listItemService);
        REQUIRE_NOTHROW(move.make(listName));
    }

    // ---- make() — duplicate ----------------------------------------------------

    SECTION("make — duplicate increases item count")
    {
        Command command("duplicate", { "aaaa" }, {}, "duplicate aaaa");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<FileDataServiceInterface> listStoragePtr = std::make_unique<JSONService>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        Move move(ioService, command, commandService, listService, listItemService);
        REQUIRE_NOTHROW(move.make(listName));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 3);

        installation.wipe();
        installation.make();
    }
}
