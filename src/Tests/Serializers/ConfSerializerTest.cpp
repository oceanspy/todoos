#define CATCH_CONFIG_MAIN
#include "../../Serializers/ConfSerializer.h"
#include "../../IOService/IOService.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>

TEST_CASE("ConfSerializer tests", "[ConfSerializer]")
{
    // Create a temporary directory for testing
    std::filesystem::path tempDir = std::filesystem::temp_directory_path() / "_todoos_conf_service_test";
    std::filesystem::create_directory(tempDir);

    // Create a temporary file path for testing
    std::filesystem::path tempFile = tempDir / "test.conf";
    std::filesystem::path wrongPermissionTempFile = "/root/test.conf";

    // Create a mock IOService for testing
    IOService ioService("cli");

    SECTION("Load method throws invalid_argument when file is not writable")
    {
        ConfSerializer confService(ioService);
        REQUIRE_THROWS_AS(confService.load(wrongPermissionTempFile), std::invalid_argument);
    }

    SECTION("Read and write methods work correctly")
    {
        ConfSerializer confService(ioService);

        // Prepare data for writing
        std::vector<std::vector<std::string>> testData = {
            { "key1", "value1" }, { "key2", "value2" }, { "key3", "value3" }, { "key4", "/path/to/some/file" }
        };

        // Write data to the file
        confService.load(tempFile);
        confService.write(testData);

        // Read data from the file
        std::vector<std::vector<std::string>> readData = confService.read(std::nullopt);

        // Verify that read data matches written data
        REQUIRE(readData == testData);
    }

    SECTION("Append method adds data to existing file")
    {
        ConfSerializer confService(ioService);
        confService.load(tempFile);

        std::vector<std::vector<std::string>> initialData = { { "key1", "value1" }, { "key2", "value2" } };
        confService.write(initialData);

        std::vector<std::vector<std::string>> appendData = { { "key3", "value3" } };
        confService.append(appendData);

        std::vector<std::vector<std::string>> readData = confService.read(std::nullopt);
        REQUIRE(readData.size() == 3);
        REQUIRE(readData[0] == std::vector<std::string>{ "key1", "value1" });
        REQUIRE(readData[1] == std::vector<std::string>{ "key2", "value2" });
        REQUIRE(readData[2] == std::vector<std::string>{ "key3", "value3" });
    }

    SECTION("Empty method clears the file")
    {
        ConfSerializer confService(ioService);
        confService.load(tempFile);

        std::vector<std::vector<std::string>> testData = { { "key1", "value1" }, { "key2", "value2" } };
        confService.write(testData);

        confService.empty();

        ConfSerializer confService2(ioService);
        confService2.load(tempFile);
        std::vector<std::vector<std::string>> readData = confService2.read(std::nullopt);
        REQUIRE(readData.empty());
    }

    SECTION("Write and read empty data")
    {
        ConfSerializer confService(ioService);
        confService.load(tempFile);

        std::vector<std::vector<std::string>> emptyData = {};
        confService.write(emptyData);

        std::vector<std::vector<std::string>> readData = confService.read(std::nullopt);
        REQUIRE(readData.empty());
    }

    // Clean up: remove temporary directory
    std::filesystem::remove_all(tempDir);
}
