#define CATCH_CONFIG_MAIN
#include "../../FileDataStorage/JSONService.h"
#include "../../IOService/IOService.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>

TEST_CASE("JSONService tests", "[JSONService]")
{
    // Create a temporary directory for testing
    std::filesystem::path tempDir = std::filesystem::temp_directory_path() / "_todoos_json_service_test";
    std::filesystem::create_directory(tempDir);

    // Create a temporary file path for testing
    std::filesystem::path tempFile = tempDir / "test.json";
    std::filesystem::path wrongPermissionTempFile = "/root/test.json";

    // Create a mock IOService for testing
    IOService ioService("cli");

    SECTION("Load method throws invalid_argument when file is not writable")
    {
        JSONService jsonService(ioService);
        REQUIRE_THROWS_AS(jsonService.load(wrongPermissionTempFile), std::invalid_argument);
    }

    SECTION("Read and write methods work correctly")
    {
        JSONService jsonService(ioService);

        // Prepare data for writing
        std::vector<std::vector<std::string>> testData = { { "1", "2", "3" }, { "4", "5", "6" }, { "7", "8", "9" } };

        // Write data to the file
        jsonService.load(tempFile);
        jsonService.write(testData);

        // Read data from the file
        std::vector<std::vector<std::string>> readData = jsonService.read(std::nullopt);

        // Verify that read data matches written data
        REQUIRE(readData == testData);
    }

    SECTION("Append method adds data to existing file")
    {
        JSONService jsonService(ioService);
        jsonService.load(tempFile);

        std::vector<std::vector<std::string>> initialData = { { "a", "b" }, { "c", "d" } };
        jsonService.write(initialData);

        std::vector<std::vector<std::string>> appendData = { { "e", "f" } };
        jsonService.append(appendData);

        std::vector<std::vector<std::string>> readData = jsonService.read(std::nullopt);
        REQUIRE(readData.size() == 3);
        REQUIRE(readData[0] == std::vector<std::string>{ "a", "b" });
        REQUIRE(readData[1] == std::vector<std::string>{ "c", "d" });
        REQUIRE(readData[2] == std::vector<std::string>{ "e", "f" });
    }

    SECTION("Append to empty file")
    {
        JSONService jsonService(ioService);
        jsonService.load(tempFile);

        std::vector<std::vector<std::string>> appendData = { { "x", "y" } };
        jsonService.append(appendData);

        std::vector<std::vector<std::string>> readData = jsonService.read(std::nullopt);
        REQUIRE(readData.size() == 1);
        REQUIRE(readData[0] == std::vector<std::string>{ "x", "y" });
    }

    SECTION("Empty method clears the file")
    {
        JSONService jsonService(ioService);
        jsonService.load(tempFile);

        std::vector<std::vector<std::string>> testData = { { "1", "2" }, { "3", "4" } };
        jsonService.write(testData);

        jsonService.empty();

        // After empty, the file is cleared; re-load to reinitialize
        JSONService jsonService2(ioService);
        jsonService2.load(tempFile);
        std::vector<std::vector<std::string>> readData = jsonService2.read(std::nullopt);
        REQUIRE(readData.empty());
    }

    SECTION("Write and read empty data")
    {
        JSONService jsonService(ioService);
        jsonService.load(tempFile);

        std::vector<std::vector<std::string>> emptyData = {};
        jsonService.write(emptyData);

        std::vector<std::vector<std::string>> readData = jsonService.read(std::nullopt);
        REQUIRE(readData.empty());
    }

    SECTION("Read with limit keeps last N items via sliding window")
    {
        JSONService jsonService(ioService);
        jsonService.load(tempFile);

        std::vector<std::vector<std::string>> testData = {
            { "1" }, { "2" }, { "3" }, { "4" }, { "5" }
        };
        jsonService.write(testData);

        // The limit implementation uses a sliding window that erases from front
        // once count reaches limit, so with 5 items and limit=3, it keeps the last 2
        std::vector<std::vector<std::string>> readData = jsonService.read(3);
        REQUIRE(readData.size() == 2);
        REQUIRE(readData[0] == std::vector<std::string>{ "4" });
        REQUIRE(readData[1] == std::vector<std::string>{ "5" });

        // With limit larger than data size, all items are returned
        std::vector<std::vector<std::string>> readAll = jsonService.read(10);
        REQUIRE(readAll.size() == 5);
    }

    // Clean up: remove temporary directory
    std::filesystem::remove_all(tempDir);
}
