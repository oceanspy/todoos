#include "../../FileDataStorageRepositories/ListRepository.h"
#include "../../FileDataStorage/ConfService.h"
#include "../../FileDataStorage/JSONService.h"
#include "../../IOService/IOService.h"
#include "../Mock/MockInit.h"
#include "../Mock/MockInstallation.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>

TEST_CASE("ListRepositoryTest", "[ListRepository]")
{
    // Create mock objects
    IOService ioService("cli");
    ConfService confService = ConfService(ioService);
    JSONService jsonService = JSONService(ioService);
    std::unique_ptr<FileDataServiceInterface> fileDataStorageServicePtr = std::make_unique<JSONService>(ioService);
    std::unique_ptr<FileDataServiceInterface> fileDataConfigStorageServicePtr =
        std::make_unique<ConfService>(ioService);
    MockInit init(ioService, "_todoos_ListRepositoryTest");
    MockInstallation installation(ioService, jsonService, confService, init);
    std::string tempListName = "tempListName";
    std::string tempList2Name = "tempList2Name";
    installation.wipe();
    installation.make();
    Command command = Command("", {}, {}, "");
    ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
    ConfigService configService(ioService, init, configRepository, cacheRepository, command);
    ListRepository listRepository(configService, fileDataStorageServicePtr.get());

    SECTION("Test get method")
    {
        std::vector<ListEntity> results = listRepository.get();

        REQUIRE(!results.empty());
        REQUIRE(results.size() == 2);
        REQUIRE(*results[0].getName() == "tempListName");
        REQUIRE(*results[0].getType() == "default");
        REQUIRE(*results[0].getSorting() == "default");
        REQUIRE(*results[0].isHidden() == false);
        REQUIRE(*results[1].getName() == "tempList2Name");
        REQUIRE(*results[1].getType() == "default");
        REQUIRE(*results[1].getSorting() == "default");
        REQUIRE(*results[1].isHidden() == false);
    }

    SECTION("Test find method")
    {
        // Call the get method
        const std::string name = "tempListName";
        const std::string theme = "theme1";
        const std::string sorting = "sorting1";

        ListEntity result = listRepository.find(name);
        REQUIRE(*result.getName() == "tempListName");
        REQUIRE(*result.getType() == "default");
        REQUIRE(*result.getSorting() == "default");
    }

    SECTION("Test update method")
    {
        const std::string name = "tempListName";
        const std::string theme = "theme1";
        const std::string sorting = "sorting1";

        ListEntity oldItem = listRepository.find(name);

        const std::string newName = "tempList3Name";
        const std::string newTheme = "theme3";
        const std::string newSorting = "sorting3";

        ListEntity newItem;
        newItem.setName(newName);
        newItem.setType(newTheme);
        newItem.setSorting(newSorting);
        newItem.setHidden(false);

        listRepository.update(name, newItem);

        ListEntity result = listRepository.find(newName);
        REQUIRE(*result.getName() == newName);
        REQUIRE(*result.getType() == newTheme);
        REQUIRE(*result.getSorting() == newSorting);

        listRepository.update(name, oldItem);

        installation.wipe();
        installation.make();
    }

    SECTION("Test create & delete method")
    {
        const std::string name = "tempListName";
        const std::string theme = "theme1";
        const std::string sorting = "sorting1";

        const std::string newName = "tempList3Name";
        const std::string newTheme = "theme3";
        const std::string newSorting = "sorting3";

        ListEntity newItem;
        newItem.setName(newName);
        newItem.setType(newTheme);
        newItem.setSorting(newSorting);
        newItem.setHidden(false);

        listRepository.create(newItem);

        ListEntity result = listRepository.find(newName);
        REQUIRE(*result.getName() == newName);
        REQUIRE(*result.getType() == newTheme);
        REQUIRE(*result.getSorting() == newSorting);

        listRepository.remove(const_cast<std::string&>(newName));

        std::vector<ListEntity> results = listRepository.get();

        REQUIRE(!results.empty());
        REQUIRE(results.size() == 2);
        REQUIRE(*results[0].getName() == "tempListName");
        REQUIRE(*results[0].getType() == "default");
        REQUIRE(*results[0].getSorting() == "default");
        REQUIRE(*results[1].getName() == "tempList2Name");
        REQUIRE(*results[1].getType() == "default");
        REQUIRE(*results[1].getSorting() == "default");

        installation.wipe();
        installation.make();
    }
}
