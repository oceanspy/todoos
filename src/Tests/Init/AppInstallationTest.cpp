#include "../../IOService/IOService.h"
#include "../../Serializers/ConfSerializer.h"
#include "../../Serializers/JsonSerializer.h"
#include "../Mock/MockAppInitialization.h"
#include "../Mock/MockAppInstallation.h"
#include "../../Init/AppInstallation.h"
#include "../../List/ListItems/ListItemEntity.h"
#include "../../Serializers/CsvSerializer.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("isNew returns true when main directory is not present", "[Installation]")
{
    // Create mock objects
    std::string channel = "cli";
    IOService ioService = IOService(channel);
    ConfSerializer confService = ConfSerializer(ioService);
    JsonSerializer jsonService = JsonSerializer(ioService);
    MockAppInitialization init(ioService, "_todoos_InstallationTest");
    MockAppInstallation installation(ioService, jsonService, confService, init);

    // Reset the main directory
    installation.wipe();

    // Check if main directory is not present before installation
    REQUIRE_FALSE(std::filesystem::exists(init.getMainDirPath()));

    // Run the installation
    REQUIRE(installation.isNew());
}

TEST_CASE("Installation creates directories and files correctly", "[Installation]")
{
    // Create mock objects
    std::string channel = "cli";
    IOService ioService = IOService(channel);
    ConfSerializer confService = ConfSerializer(ioService);
    JsonSerializer jsonService = JsonSerializer(ioService);
    MockAppInitialization init(ioService, "_todoos_InstallationTest");
    MockAppInstallation installation(ioService, jsonService, confService, init);

    // Reset the main directory
    installation.wipe();

    // Check if directories and files are not present before installation
    REQUIRE_FALSE(std::filesystem::exists(init.getMainDirPath()));
    REQUIRE_FALSE(std::filesystem::exists(init.getConfigDirPath()));
    REQUIRE_FALSE(std::filesystem::exists(init.getAppDirPath()));
    REQUIRE_FALSE(std::filesystem::exists(init.getConfigFilePath()));
    REQUIRE_FALSE(std::filesystem::exists(init.getDefaultListFilePath()));

    // Run the installation
    installation.make();

    // Check if directories and files are present after installation
    REQUIRE(std::filesystem::exists(init.getMainDirPath()));
    REQUIRE(std::filesystem::exists(init.getConfigDirPath()));
    REQUIRE(std::filesystem::exists(init.getAppDirPath()));
    REQUIRE(std::filesystem::exists(init.getConfigFilePath()));
    REQUIRE(std::filesystem::exists(init.getDefaultListFilePath()));
}

TEST_CASE("Populate default list file", "[Installation]")
{
    // Create mock objects
    std::string channel = "cli";
    IOService ioService = IOService(channel);
    ConfSerializer confService = ConfSerializer(ioService);
    JsonSerializer jsonService = JsonSerializer(ioService);
    MockAppInitialization init(ioService, "_todoos_InstallationTest");
    MockAppInstallation installation(ioService, jsonService, confService, init);

    // Reset the main directory
    installation.wipe();

    // Check if default list file is not present before installation
    REQUIRE_FALSE(std::filesystem::exists(init.getDefaultListFilePath()));

    // Run the installation
    installation.make();

    // Check if default list file is present after installation
    REQUIRE(std::filesystem::exists(init.getDefaultListFilePath()));

    // Check if default list file is populated correctly
    jsonService.load(init.getDefaultListFilePath());
    std::vector<std::vector<std::string>> defaultListData = jsonService.read(std::nullopt);
    REQUIRE(defaultListData.size() == 2);
    REQUIRE(defaultListData[0][0] == "aaaa");
    REQUIRE(defaultListData[0][1] == "");
    REQUIRE(defaultListData[0][2] == "test 1");
    REQUIRE(defaultListData[0][3] == "high");
    REQUIRE(defaultListData[0][4] == "queued");
    REQUIRE(defaultListData[0][5] == "0");
    REQUIRE(defaultListData[0][6] == "0");
    REQUIRE(defaultListData[0][7] == "1712487259");
    REQUIRE(defaultListData[0][8] == "1712487259");
    REQUIRE(defaultListData[1][0] == "bbbb");
    REQUIRE(defaultListData[1][1] == "");
    REQUIRE(defaultListData[1][2] == "test 2");
    REQUIRE(defaultListData[1][3] == "medium");
    REQUIRE(defaultListData[1][4] == "started");
    REQUIRE(defaultListData[1][5] == "1712487259");
    REQUIRE(defaultListData[1][6] == "0");
    REQUIRE(defaultListData[1][7] == "1712487272");
    REQUIRE(defaultListData[1][8] == "1712487272");
}

TEST_CASE("Real installation writes welcome items with the current row layout", "[Installation]")
{
    IOService ioService = IOService("cli");
    ConfSerializer confService = ConfSerializer(ioService);
    JsonSerializer jsonService = JsonSerializer(ioService);
    CsvSerializer csvService = CsvSerializer(ioService);
    MockAppInitialization init(ioService, "_todoos_RealInstallationTest");
    AppInstallation installation(ioService, jsonService, csvService, confService, init);

    installation.wipe();
    installation.make();

    CsvSerializer reader = CsvSerializer(ioService);
    reader.load(init.getDefaultListFilePath());
    std::vector<std::vector<std::string>> rows = reader.read(0);
    REQUIRE(rows.size() == 4);
    for (const std::vector<std::string>& row : rows) {
        REQUIRE(row.size() == ListItemEntity::ROW_SIZE);
        REQUIRE(row[1].empty());
    }

    installation.wipe();
}
