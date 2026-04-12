#include "../../Actions/ConfigAction/ConfigAction.h"
#include "../../Command/CommandRegistry.h"
#include "../../Command/CommandOption.h"
#include "../../Command/CommandService.h"
#include "../../Config/ConfigService.h"
#include "../../Events/EventBus.h"
#include "../../Serializers/ConfSerializer.h"
#include "../../Serializers/JsonSerializer.h"
#include "../../FileDataStorageRepositories/ConfigRepository.h"
#include "../../FileDataStorageRepositories/ListRepository.h"
#include "../../List/ListService.h"
#include "../Mock/MockAppInitialization.h"
#include "../Mock/MockAppInstallation.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Config action", "[Config]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);

    MockAppInitialization init(ioService, "_todoos_ConfigActionTest");
    MockAppInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    CommandRegistry commandList;
    CommandOption commandOption;
    CommandService commandService(commandList, commandOption);

    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());

    // ---- make() — no subcommand (showAll) --------------------------------------

    SECTION("make — no subcommand shows config without throwing")
    {
        Command command("config", {}, {}, "config");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        ConfigAction config(ioService, command, commandService, configService, listService);
        REQUIRE_NOTHROW(config.make());
    }

    // ---- make() — edit defaultList ---------------------------------------------

    SECTION("make — edit defaultList to valid list updates config")
    {
        Command command("config", { "edit", "defaultList", "tempListName" }, {}, "config edit defaultList tempListName");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        ConfigAction config(ioService, command, commandService, configService, listService);
        REQUIRE_NOTHROW(config.make());
        REQUIRE(configService.getValue("defaultList") == "tempListName");
    }

    SECTION("make — edit defaultList to non-existent list does not throw")
    {
        Command command("config",
                        { "edit", "defaultList", "nonExistentList" },
                        {},
                        "config edit defaultList nonExistentList");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        ConfigAction config(ioService, command, commandService, configService, listService);
        REQUIRE_NOTHROW(config.make());
    }

    // ---- make() — edit theme ---------------------------------------------------

    SECTION("make — edit theme to valid value does not throw")
    {
        Command command("config", { "edit", "theme", "default" }, {}, "config edit theme default");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        ConfigAction config(ioService, command, commandService, configService, listService);
        REQUIRE_NOTHROW(config.make());
    }

    SECTION("make — edit theme to invalid value does not throw")
    {
        Command command("config", { "edit", "theme", "badTheme" }, {}, "config edit theme badTheme");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        ConfigAction config(ioService, command, commandService, configService, listService);
        REQUIRE_NOTHROW(config.make());
    }

    // ---- make() — edit unsupported key -----------------------------------------

    SECTION("make — edit unsupported key does not throw")
    {
        Command command("config", { "edit", "unknownKey", "value" }, {}, "config edit unknownKey value");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        ConfigAction config(ioService, command, commandService, configService, listService);
        REQUIRE_NOTHROW(config.make());
    }

    SECTION("make — edit with missing value argument does not throw")
    {
        Command command("config", { "edit" }, {}, "config edit");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        ConfigAction config(ioService, command, commandService, configService, listService);
        REQUIRE_NOTHROW(config.make());
    }

    // ---- make() — invalid subcommand -------------------------------------------

    SECTION("make — invalid subcommand throws std::invalid_argument")
    {
        Command command("config", { "badsubcmd" }, {}, "config badsubcmd");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        ConfigAction config(ioService, command, commandService, configService, listService);
        REQUIRE_THROWS_AS(config.make(), std::invalid_argument);
    }
}
