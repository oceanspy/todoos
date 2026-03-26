#include "../../Config/ConfigService.h"
#include "../../FileDataStorage/ConfService.h"
#include "../../FileDataStorage/JSONService.h"
#include "../Mock/MockInit.h"
#include "../Mock/MockInstallation.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ConfigServiceTest", "[ConfigService]")
{
    IOService ioService("cli");
    ConfService confService = ConfService(ioService);
    JSONService jsonService = JSONService(ioService);
    std::unique_ptr<FileDataServiceInterface> fileDataConfigStorageServicePtr =
        std::make_unique<ConfService>(ioService);
    MockInit init(ioService, "_todoos_ConfigServiceTest");
    MockInstallation installation(ioService, jsonService, confService, init);

    installation.wipe();
    installation.make();

    Command command = Command("", {}, {}, "");
    ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
    ConfigService configService(ioService, init, configRepository, cacheRepository, command);

    SECTION("get returns all config entries")
    {
        std::vector<ConfigEntity> configs = configService.get();
        REQUIRE(configs.size() > 0);
    }

    SECTION("getValue returns value for existing key")
    {
        std::string value = configService.getValue("defaultList");
        REQUIRE(value.empty() == false);
    }

    SECTION("getValue returns empty for non-existent key")
    {
        std::string value = configService.getValue("nonExistentKey");
        REQUIRE(value == "");
    }

    SECTION("isTrue returns true for 'true' values")
    {
        configService.edit("archiveWhenCompleted", "true");
        REQUIRE(configService.isTrue("archiveWhenCompleted") == true);

        configService.edit("archiveWhenCompleted", "false");
        REQUIRE(configService.isTrue("archiveWhenCompleted") == false);
    }

    SECTION("find returns entity for existing key")
    {
        ConfigEntity entity = configService.find("defaultList");
        REQUIRE(*entity.getKey() == "defaultList");
        REQUIRE(entity.getValue() != nullptr);
    }

    SECTION("find throws for non-existent key")
    {
        REQUIRE_THROWS_AS(configService.find("nonExistentKey"), std::invalid_argument);
    }

    SECTION("edit modifies existing config")
    {
        configService.edit("theme", "light");
        ConfigEntity entity = configService.find("theme");
        REQUIRE(*entity.getValue() == "light");

        // Restore
        configService.edit("theme", "default");
    }

    SECTION("getFileDataStorageType")
    {
        std::string type = configService.getFileDataStorageType();
        REQUIRE(type.empty() == false);
    }

    SECTION("getDefaultList")
    {
        std::string list = configService.getDefaultList();
        REQUIRE(list.empty() == false);
    }

    SECTION("getAppDirPath")
    {
        std::filesystem::path path = configService.getAppDirPath();
        REQUIRE(path.empty() == false);
    }

    SECTION("getUsedListNameStr returns default list when no command option")
    {
        std::string listName = configService.getUsedListNameStr();
        REQUIRE(listName.empty() == false);
    }

    SECTION("getUsedListVariantStr returns default when no option")
    {
        std::string variant = configService.getUsedListVariantStr();
        REQUIRE(variant == "default");
    }

    SECTION("getUsedListVariantStr returns archive when command has archive option")
    {
        std::map<std::string, std::string> options = { { "archive", "" } };
        Command archiveCommand = Command("show", {}, options, "show --archive");
        ConfigService archiveConfigService(ioService, init, configRepository, cacheRepository, archiveCommand);

        std::string variant = archiveConfigService.getUsedListVariantStr();
        REQUIRE(variant == "archive");
    }

    SECTION("getUsedListVariantStr returns delete when command has delete option")
    {
        std::map<std::string, std::string> options = { { "delete", "" } };
        Command deleteCommand = Command("show", {}, options, "show --delete");
        ConfigService deleteConfigService(ioService, init, configRepository, cacheRepository, deleteCommand);

        std::string variant = deleteConfigService.getUsedListVariantStr();
        REQUIRE(variant == "delete");
    }

    SECTION("getUsedListNameStr uses command option when present")
    {
        std::map<std::string, std::string> options = { { "list", "myList" } };
        Command listCommand = Command("show", {}, options, "show --list myList");
        ConfigService listConfigService(ioService, init, configRepository, cacheRepository, listCommand);

        std::string listName = listConfigService.getUsedListNameStr();
        REQUIRE(listName == "myList");
    }

    SECTION("add with unknown key throws")
    {
        REQUIRE_THROWS_AS(configService.add("unknownKey", "testValue"), std::invalid_argument);
    }

    SECTION("editCurrentList updates cache")
    {
        std::string originalList = configService.getUsedListNameStr();
        configService.editCurrentList("newListValue");
        REQUIRE(configService.getUsedListNameStr() == "newListValue");
        // Restore
        configService.editCurrentList(originalList);
    }

    SECTION("getDefaultSystemExtension returns non-empty")
    {
        std::string ext = configService.getDefaultSystemExtension();
        REQUIRE(ext.empty() == false);
    }

    SECTION("getListofListFilePath returns non-empty")
    {
        std::filesystem::path path = configService.getListofListFilePath();
        REQUIRE(path.empty() == false);
    }

    SECTION("getCurrentListFilePath returns non-empty")
    {
        std::filesystem::path path = configService.getCurrentListFilePath();
        REQUIRE(path.empty() == false);
    }

    SECTION("getListFilePath")
    {
        std::filesystem::path path = configService.getListFilePath("testList");
        REQUIRE(path.string().find("testList") != std::string::npos);
    }

    SECTION("getListArchiveFilePath")
    {
        std::filesystem::path path = configService.getListArchiveFilePath("testList");
        REQUIRE(path.string().find(".archive_testList") != std::string::npos);
    }

    SECTION("getListDeleteFilePath")
    {
        std::filesystem::path path = configService.getListDeleteFilePath("testList");
        REQUIRE(path.string().find(".del_testList") != std::string::npos);
    }

    SECTION("getListArchiveFilePathFromFilePath")
    {
        std::filesystem::path input = "/some/path/mylist.json";
        std::filesystem::path result = configService.getListArchiveFilePathFromFilePath(input);
        REQUIRE(result.string().find(".archive_mylist.json") != std::string::npos);

        // Already an archive path should return as-is
        std::filesystem::path archiveInput = "/some/path/.archive_mylist.json";
        std::filesystem::path archiveResult = configService.getListArchiveFilePathFromFilePath(archiveInput);
        REQUIRE(archiveResult == archiveInput);
    }

    SECTION("getListDeleteFilePathFromFilePath")
    {
        std::filesystem::path input = "/some/path/mylist.json";
        std::filesystem::path result = configService.getListDeleteFilePathFromFilePath(input);
        REQUIRE(result.string().find(".del_mylist.json") != std::string::npos);

        // Already a delete path should return as-is
        std::filesystem::path delInput = "/some/path/.del_mylist.json";
        std::filesystem::path delResult = configService.getListDeleteFilePathFromFilePath(delInput);
        REQUIRE(delResult == delInput);
    }
}
