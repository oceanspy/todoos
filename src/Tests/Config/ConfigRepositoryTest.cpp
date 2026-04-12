#include "../../FileDataStorageRepositories/ConfigRepository.h"
#include "../../Serializers/ConfSerializer.h"
#include "../../Serializers/JsonSerializer.h"
#include "../../IOService/IOService.h"
#include "../Mock/MockAppInitialization.h"
#include "../Mock/MockAppInstallation.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>

TEST_CASE("ConfigRepositoryTest", "[ConfigRepository]")
{
    // Create mock objects
    IOService ioService("cli");
    ConfSerializer confService = ConfSerializer(ioService);
    JsonSerializer jsonService = JsonSerializer(ioService);
    std::unique_ptr<DataSerializerInterface> fileDataStorageServicePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> fileDataConfigStorageServicePtr =
        std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_ConfigRepositoryTest");
    MockAppInstallation installation(ioService, jsonService, confService, init);
    std::string tempListName = "tempListName";
    std::string tempList2Name = "tempList2Name";
    installation.wipe();
    installation.make();
    ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());

    SECTION("Test get method")
    {
        // Call the get method
        std::vector<ConfigEntity> results = configRepository.get();

        REQUIRE(!results.empty());
        REQUIRE(results.size() == 7);
        REQUIRE(*configRepository.find("fileDataStorageType").getValue() == "json");
        REQUIRE(*configRepository.find("appDirStorage").getValue() == init.getMainDirPath());
        REQUIRE(*configRepository.find("defaultList").getValue() == "default");
        REQUIRE(*configRepository.find("theme").getValue() == "default");
        REQUIRE(*configRepository.find("consoleRowMaxLength").getValue() == "96");
        REQUIRE(*configRepository.find("archiveWhenCompleted").getValue() == "false");
        REQUIRE(*configRepository.find("idRandomGenerationType").getValue() == "letters");
    }

    SECTION("Test find method: config")
    {
        // Call the get method
        ConfigEntity result = configRepository.find("theme");

        REQUIRE(*result.getKey() == "theme");
        REQUIRE(*result.getValue() == "default");
    }

    SECTION("Test update method")
    {
        ConfigEntity oldConfig = configRepository.find("theme");

        const std::string key = "theme";
        const std::string value = "newValue";
        // Call the get method
        ConfigEntity newConfig;
        newConfig.setKey(key);
        newConfig.setValue(value);
        configRepository.update("theme", newConfig);

        ConfigEntity result = configRepository.find(key);

        REQUIRE(*result.getKey() == "theme");
        REQUIRE(*result.getValue() == "newValue");

        configRepository.update("theme", oldConfig);
    }

    SECTION("Test create & delete method")
    {
        const std::string key = "newConfig";
        const std::string value = "newValue";
        // Call the get method
        ConfigEntity newConfig;
        newConfig.setKey(key);
        newConfig.setValue(value);
        configRepository.create(newConfig);

        ConfigEntity result = configRepository.find(key);

        REQUIRE(*result.getKey() == key);
        REQUIRE(*result.getValue() == value);

        configRepository.remove(const_cast<std::string&>(key));

        std::vector<ConfigEntity> results = configRepository.get();
        REQUIRE(!results.empty());
        REQUIRE(results.size() == 7);
    }
}
