#include "../../Config/ConfigService.h"
#include "../../FileDataStorageRepositories/DescriptionRepository.h"
#include "../../FileDataStorageRepositories/ListItemRepository.h"
#include "../../FileDataStorageRepositories/ListRepository.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"
#include "../../List/ListService.h"
#include "../../Serializers/ConfSerializer.h"
#include "../../Serializers/JsonSerializer.h"
#include "../Mock/MockAppInitialization.h"
#include "../Mock/MockAppInstallation.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>

// List storage lifecycle (create / rename / remove), driven through ListService
// and backed by the file-based ListRepository.
TEST_CASE("FileStorage", "[FileStorage][ListRepository]")
{
    IOService ioService("cli");
    ConfSerializer confService = ConfSerializer(ioService);
    JsonSerializer jsonService = JsonSerializer(ioService);
    std::unique_ptr<DataSerializerInterface> fileDataStorageServicePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> fileDataConfigStorageServicePtr =
        std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_FileStorageTest");
    MockAppInstallation installation(ioService, jsonService, confService, init);
    Command command = Command("commands", {}, {}, "commands");
    ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
    ConfigService configService(ioService, init, configRepository, cacheRepository, command);
    EventBus bus = EventBus();

    installation.wipe();
    installation.make();

    PriorityService priorityService = PriorityService();
    StatusService statusService = StatusService();
    ListItemRepository listItemRepository(
        configService, fileDataStorageServicePtr.get(), priorityService, statusService);
    DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());

    ListItemService listItemService(
        ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
    ListRepository listRepository(configService, fileDataStorageServicePtr.get());
    ListService listService(ioService, configService, listRepository, bus);
    ListName listName = listService.createListName("tempListName");

    auto allFilesExist = [&](const std::string& name) {
        return std::filesystem::exists(configService.getListFilePath(name)) &&
               std::filesystem::exists(configService.getListArchiveFilePath(name)) &&
               std::filesystem::exists(configService.getListDeleteFilePath(name));
    };
    auto anyFileExists = [&](const std::string& name) {
        return std::filesystem::exists(configService.getListFilePath(name)) ||
               std::filesystem::exists(configService.getListArchiveFilePath(name)) ||
               std::filesystem::exists(configService.getListDeleteFilePath(name));
    };

    SECTION("rename moves the list entry and its files")
    {
        REQUIRE(listItemService.get(listName).size() == 2);

        REQUIRE_NOTHROW(listService.edit("tempListName", "tempListName-new"));

        REQUIRE_NOTHROW(listService.find("tempListName-new"));
        REQUIRE_THROWS(listService.find("tempListName"));
        REQUIRE(allFilesExist("tempListName-new"));
        REQUIRE_FALSE(anyFileExists("tempListName"));

        ListName listNameNew = listService.createListName("tempListName-new");
        REQUIRE(listItemService.get(listNameNew).size() == 2);
    }

    SECTION("rename onto an existing list throws and changes nothing")
    {
        REQUIRE_THROWS(listService.edit("tempListName", "tempList2Name"));

        REQUIRE_NOTHROW(listService.find("tempListName"));
        REQUIRE_NOTHROW(listService.find("tempList2Name"));
        REQUIRE(listItemService.get(listName).size() == 2);
    }

    SECTION("rename with an invalid name throws before touching files")
    {
        REQUIRE_THROWS(listService.edit("tempListName", "invalid name"));

        REQUIRE(allFilesExist("tempListName"));
        REQUIRE_NOTHROW(listService.find("tempListName"));
    }

    SECTION("add creates the list entry and its files")
    {
        REQUIRE_NOTHROW(listService.add("tempListName-new-create"));

        REQUIRE_NOTHROW(listService.find("tempListName-new-create"));
        REQUIRE(allFilesExist("tempListName-new-create"));

        ListName listNameNew = listService.createListName("tempListName-new-create");
        listItemService.add(listNameNew, "auto");
        REQUIRE(listItemService.get(listNameNew).size() == 1);
    }

    SECTION("remove deletes the list entry and its files")
    {
        REQUIRE_NOTHROW(listService.remove("tempListName"));

        REQUIRE_THROWS(listService.find("tempListName"));
        REQUIRE_FALSE(anyFileExists("tempListName"));
    }

    SECTION("removeKeepingBackup deletes the list entry and keeps its files as a backup")
    {
        REQUIRE_NOTHROW(listService.removeKeepingBackup("tempListName"));

        std::string backupName = ListRepository::BACKUP_PREFIX + "tempListName";
        REQUIRE_THROWS(listService.find("tempListName"));
        REQUIRE_FALSE(anyFileExists("tempListName"));
        REQUIRE(allFilesExist(backupName));
    }

    SECTION("remove of an unknown list throws")
    {
        REQUIRE_THROWS(listService.remove("unknownList"));
        REQUIRE_THROWS(listService.removeKeepingBackup("unknownList"));
    }

    installation.wipe();
}
