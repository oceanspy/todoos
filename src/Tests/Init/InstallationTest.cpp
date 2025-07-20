#include "../../FileDataStorage/ConfService.h"
#include "../../FileDataStorage/JSONService.h"
#include "../../IOService/IOService.h"
#include "../Mock/MockInit.h"
#include "../Mock/MockInstallation.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("isNew returns true when main directory is not present", "[Installation]")
{
    // Create mock objects
    std::string channel = "cli";
    IOService ioService = IOService(channel);
    ConfService confService = ConfService(ioService);
    JSONService jsonService = JSONService(ioService);
    MockInit init(ioService, "_todoos_InstallationTest");
    MockInstallation installation(ioService, jsonService, confService, init);

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
    ConfService confService = ConfService(ioService);
    JSONService jsonService = JSONService(ioService);
    MockInit init(ioService, "_todoos_InstallationTest");
    MockInstallation installation(ioService, jsonService, confService, init);

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
    ConfService confService = ConfService(ioService);
    JSONService jsonService = JSONService(ioService);
    MockInit init(ioService, "_todoos_InstallationTest");
    MockInstallation installation(ioService, jsonService, confService, init);

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
    // {"aaaa", "test 1", "3", "0", "1712487259", "1712487259"},
    // {"bbbb", "test 2", "1", "1", "1712487272", "1712487272"}
    REQUIRE(defaultListData[0][0] == "aaaa");
    REQUIRE(defaultListData[0][1] == "test 1");
    REQUIRE(defaultListData[0][2] == "high");
    REQUIRE(defaultListData[0][3] == "to-do");
    REQUIRE(defaultListData[0][4] == "0");
    REQUIRE(defaultListData[0][5] == "0");
    REQUIRE(defaultListData[0][6] == "1712487259");
    REQUIRE(defaultListData[0][7] == "1712487259");
    REQUIRE(defaultListData[1][0] == "bbbb");
    REQUIRE(defaultListData[1][1] == "test 2");
    REQUIRE(defaultListData[1][2] == "medium");
    REQUIRE(defaultListData[1][3] == "started");
    REQUIRE(defaultListData[1][4] == "1712487259");
    REQUIRE(defaultListData[1][5] == "0");
    REQUIRE(defaultListData[1][6] == "1712487272");
    REQUIRE(defaultListData[1][7] == "1712487272");
}
